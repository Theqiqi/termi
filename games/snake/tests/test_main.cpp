#include "test_h.h"
#include "test_ai.h"
int main() {
    TestCollision();
    TestScoring();
    TestSuicide();
    TestAlignment();
    TestScreenFull();
    TestAutoPlay();
    std::cout << "\n>>> ALL LOGIC TESTS PASSED! <<<\n" << std::endl;
    return 0;
}