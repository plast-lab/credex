package clsinline.trivial;


import static org.junit.Assert.assertEquals;
import org.junit.Test;


public class TrivialTest
{

    @Test
    public void testOne() {
	HostClass h = new HostClass();
	assertEquals("hello world", h.greeting(1));
    }

    @Test
    public void testZero() {
	HostClass h = new HostClass();
	assertEquals("world", h.greeting(0));
    }

    @Test
    public void testTwo() {
	HostClass h = new HostClass();
	assertEquals("hello hello world", h.greeting(2));
    }

}
