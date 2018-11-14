
package clue.testing;

import static org.junit.Assert.assertEquals;
import org.junit.Test;

public class SampleClass
{
    
    @Test
    public void myFirstTest() {
        assertEquals(2, 1 + 1);
    }

    public static String greeting() {
		return "Hello world";
	}

    public static void main(String[] args) {
	System.out.println(greeting());
    }
}
