#include "backend.h"

void save_button(void* menu) {
    RadialMenu* m = static_cast<RadialMenu*>(menu);
    std::cout << "save button was pressed" << std::endl;
    std::cout << "current: " << m->current_rotation << std::endl;
    m->current_rotation = 2.0f;
    std::cout << "new: " << m->current_rotation << std::endl;
}