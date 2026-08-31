#include "lifecycle/first_run_policy.h"

#include <cstdlib>
#include <iostream>

namespace {
    void require(bool condition, const char* message)
    {
        if (!condition) {
            std::cerr << message << '\n';
            std::exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    using SpellHotbar::Lifecycle::should_run_first_initialization;

    require(should_run_first_initialization(true, false), "new game must initialize without settings");
    require(should_run_first_initialization(true, true), "new game must initialize even after another save");
    require(should_run_first_initialization(false, false), "legacy save without HOTB must initialize");
    require(!should_run_first_initialization(false, true), "existing settings must never be overwritten");
    return EXIT_SUCCESS;
}
