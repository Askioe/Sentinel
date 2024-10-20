# Sentinel
Type 2 Intel Hypervisor.
Currently in the process of refactoring the codebase, reimplementing and cleaning up the structures.

# TODO
* Implement EPT
* Implement VmExit Handles
* Limit triggers for VmExits

# Resources
* https://revers.engineering/7-days-to-virtualization-a-series-on-hypervisor-development/
Amazing documentation on where to begin, and goes greatly indepth for developing a simple barebone hypervisor
* https://github.com/jonomango/hv
Well done resource for stealthly hiding the hypervisor to avoid detection. Perfect resource to understand EPT
* https://github.com/SamuelTulach/memhv/tree/main
Perfect layout for a hypervisor, basing my project structure based off of SamuelTulach's Hypervisor