#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <wdm.h>
#include <intrin.h>
#include <cstdint>
#include <cstdarg>


#include "util.h"

#include "VMX/Arc/cr.h"
#include "VMX/Arc/rflags.h"
#include "VMX/Arc/msr.h"

#include "VMX/Defines/VMX_Intrin.h"
#include "VMX/Defines/VMX_ControlArea.h"
#include "VMX/Defines/VMX_Guest.h"
#include "VMX/Defines/VMX_VmExitInterrupt.h"
#include "VMX/Handlers/VmExitHandlers.h"


#include "VMX/Vmcs.h"
#include "VMX/vmx.h"
