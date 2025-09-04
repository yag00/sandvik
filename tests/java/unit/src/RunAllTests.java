public class RunAllTests {
    public static void main(String[] args) {
        System.out.println("== Running TestInterfaces ==");
        TestInterfaces.main(new String[]{});

        System.out.println("== Running TestDefaultMethods ==");
        TestDefaultMethods.main(new String[]{});

        System.out.println("== Running TestAbstractClass ==");
        TestAbstractClass.main(new String[]{});

        System.out.println("== Running TestDiamond ==");
        TestDiamond.main(new String[]{});

        System.out.println("== Running TestMultiLevel ==");
        TestMultiLevel.main(new String[]{});

        System.out.println("== Running TestInterfaceConstants ==");
        TestInterfaceConstants.main(new String[]{});

        System.out.println("== Running TestInterfaceStatic ==");
        TestInterfaceStatic.main(new String[]{});

        System.out.println("== Running TestMethodConflict ==");
        TestMethodConflict.main(new String[]{});

        System.out.println("== Running TestAnonymous ==");
        TestAnonymous.main(new String[]{});

        System.out.println("== Running TestInterfaceArray ==");
        TestInterfaceArray.main(new String[]{});

        System.out.println("== Running TestArithmetic ==");
        TestArithmetic.main(new String[]{});

        System.out.println("== Running TestArrays ==");
        TestArrays.main(new String[]{});

        System.out.println("== Running TestExceptions ==");
        TestExceptions.main(new String[]{});

        System.out.println("== Running TestLoops ==");
        TestLoops.main(new String[]{});

        System.out.println("== Running TestStrings ==");
        TestStrings.main(new String[]{});

        System.out.println("== Running TestStatics ==");
        TestStatics.main(new String[]{});

        System.out.println("== Running TestInstanceOf==");
        TestInstanceOf.main(new String[]{});

        System.out.println("== Running TestPolymorphism==");
        TestPolymorphism.main(new String[]{});

        System.out.println("== Running TestFinal==");
        TestFinal.main(new String[]{});

        System.out.println("== Running TestEdgeNumbers==");
        TestEdgeNumbers.main(new String[]{});

        System.out.println("== Running TestSwitchCase==");
        TestSwitchCase.main(new String[]{});

        System.out.println("== Running TestLargeSwitchDense==");
        TestLargeSwitchDense.main(new String[]{});

        System.out.println("== Running TestLargeSwitchSparse==");
        TestLargeSwitchSparse.main(new String[]{});

        System.out.println("== Running TestDeepLoops==");
        TestDeepLoops.main(new String[]{});
    }
}
