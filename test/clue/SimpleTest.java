package clue.testing;

public class SimpleTest
{
	public int callee(int x, int y){
		return x += y;
	}

	public void caller() {
		SimpleTest st = new SimpleTest();
		st.callee(1, 2);
	}
}