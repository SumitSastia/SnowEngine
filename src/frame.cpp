#include <frame.h>

namespace frameBuffers {

    const unsigned int get_defaultVAO() {
        return gfx::internal::Screen::getVAO();
    }
}