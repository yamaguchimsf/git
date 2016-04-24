package testbench1;

public class FibBench {
	private static int doTest(int num){
		int out;
		out = fib(num);
		return out;
	}

	private static int fib(int num){
		if (num < 0) {
			return 0;
		} else if (num == 0){
			return 0;
		} else if (num == 1){
			return 1;
		} else{
			return (fib(num - 1) + fib(num - 2));
		}
	}

	public static void main(String[] args) throws Exception{
		int num = Integer.parseInt(args[0]);
		System.out.println("start fib bench");
		System.out.println("num = " + num);
		int out;
		double then = System.currentTimeMillis();
		out = doTest(num);
		double now = System.currentTimeMillis();
		System.out.println("output = " + out);
		System.out.println("time = " + (now - then));
	}
}
