#include "Engine.hpp"

int main() {
    Engine eng;

    if (!eng.init()) {
        return 404;
    }

    eng.run();
    eng.shutdown();

    return 0;
}