__int64 __fastcall CustomPsGetProcessPeb(PEPROCESS Process)
{
  	// _PEB* Peb = EProcess->Peb; //0x550 (Windows 11 | 23H2)
	return *(_QWORD*)((__int64)Process + 0x550);
}

NTSTATUS GetPEB(CONST PIODATA Data)
{
	PEPROCESS TargetProcess = NULL;

	if (Message->ProcessId == 0) return STATUS_UNSUCCESSFUL;

	NTSTATUS Status = PsLookupProcessByProcessId(Data->ProcessId, &TargetProcess);
  
	if (Status != STATUS_SUCCESS) return STATUS_UNSUCCESSFUL;

	__int64 PEBAddress = CustomPsGetProcessPeb(TargetProcess);

	if (!PEBAddress) return STATUS_UNSUCCESSFUL;

	CopyParams(Data->pOut, &PEBAddress, sizeof(PEBAddress));

	return STATUS_SUCCESS;
}

uint64 GetModuleBaseAddress(PEB _PEB, const std::wstring& ModuleName)
{
	PEB_LDR_DATA Ldr{};
	ReadMemory(_PEB.Ldr, &Ldr, sizeof(Ldr));

	LIST_ENTRY* FirstEntry = Ldr.InLoadOrderModuleList.Flink;
	
	LIST_ENTRY* CurrentEntry = FirstEntry;

	do {
		LDR_DATA_TABLE_ENTRY ModuleEntry{};
		ReadMemory(CONTAINING_RECORD(CurrentEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks), &ModuleEntry, sizeof(ModuleEntry));

		WCHAR moduleEntryName[MAX_PATH];
		ReadMemory(ModuleEntry.BaseDllName.Buffer, moduleEntryName, ModuleEntry.BaseDllName.MaximumLength);

		if (ModuleName.compare(moduleEntryName) == 0)
		{
			return reinterpret_cast<uint64>(ModuleEntry.DllBase);
		}

		CurrentEntry = ModuleEntry.InLoadOrderLinks.Flink;

	} while (CurrentEntry != FirstEntry);

	return NULL;
}

