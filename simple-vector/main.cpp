#include "simple_vector.h"

int main() {
    SimpleVectorTest::Test1();
    SimpleVectorTest::Test2();
    SimpleVectorTest::TestReserveConstructor();
    SimpleVectorTest::TestReserveMethod();
    SimpleVectorTest::TestNamedMoveConstructor();
    SimpleVectorTest::TestTemporaryObjConstructor();
    SimpleVectorTest::TestTemporaryObjOperator();
    SimpleVectorTest::TestNamedMoveOperator();
    SimpleVectorTest::TestNoncopiableMoveConstructor();
    SimpleVectorTest::TestNoncopiablePushBack();
    SimpleVectorTest::TestNoncopiableInsert();
    SimpleVectorTest::TestNoncopiableErase();
}
