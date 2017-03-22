#include "MinHook.h"

/*
* instance: pointer to an instance of a class
* hook: function to overwrite with
* offset: 0 = method 1, 1 = method 2 etc...
* return: original function
*/

void* vtablehook(void* instance, void* hook, int offset) {
        intptr_t vtable = *((intptr_t*)instance);
        intptr_t entry = vtable + sizeof(intptr_t) * offset;
        void* target = (void*)*((intptr_t*) entry);
		void* original = nullptr;

		MH_Initialize();
		MH_CreateHook(target, hook, &original);
		MH_EnableHook(target);

        return original;
}