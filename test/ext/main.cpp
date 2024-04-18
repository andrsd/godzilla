#include "godzilla/Extension.h"

class TestExtension : public godzilla::Extension {
public:
    TestExtension();
};

TestExtension::TestExtension() : godzilla::Extension("test_extension") {}

TestExtension ext;

/// Extension entry point
EXTENSION_API
godzilla::Extension *
extension_entry_point()
{
    return &ext;
}

/// Simple function for testing
EXTENSION_API
double
add(double a, double b)
{
    return a + b;
}
