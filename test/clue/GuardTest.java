package clue.testing;

public class GuardTest extends SimpleTest
{
	public int callee(int x, int y){
		return x -= y;
	}

	public void caller() {
		SimpleTest obj = new GuardTest();
		obj.callee(5, 1);
	}
}