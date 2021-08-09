#include "app.h"

int main(int argc, char const *argv[]) {
    app vkApp;
    try{vkApp.run();}catch(const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
