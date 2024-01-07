# Iterate-Module-External
Iterate Modules From Another Process Without Opening A Handle Or Attaching

```
  uint64 PEBAddress = NULL;
	API::GetPEB(ProcessId, &PEBAddress);

	PEB ProcessEnvBlock = ReadMemory<PEB>(PEBAddress);
	
	GameAssembly = GetModuleBaseAddress(ProcessEnvBlock, L"GameAssembly.dll");
```
