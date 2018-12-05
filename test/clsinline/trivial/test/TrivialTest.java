package clsinline.trivial;


//import static org.junit.Assert.assertEquals;
import static org.junit.Assert.*;
import org.junit.Test;
import java.lang.reflect.*;

public class TrivialTest
{

    @Test
    public void test_greeting() {
        HostClass h = new HostClass();
	assertEquals("hello world", h.greeting(1));
        assertEquals("world", h.greeting(0));
        assertEquals("hello hello world", h.greeting(2));
    }

    @Test
    public void test_hasattr() {
	HostClass h = new HostClass();
	Class<HostClass> cls = HostClass.class;
	try{
	    Field fld_value = cls.getDeclaredField("value");
	    assertEquals(String.class, fld_value.getType());
	    assertEquals(Modifier.PRIVATE, fld_value.getModifiers());
	} catch(NoSuchFieldException e) {
	    fail("No such field");
	}
    }

    @Test
    public void test_hasmeth() {
	HostClass h = new HostClass();
	Class<HostClass> cls = HostClass.class;
	try{
	    Method mth_addtimes = cls.getDeclaredMethod("addtimes",int.class);

	    assertEquals(String.class, mth_addtimes.getReturnType());
	    assertEquals(Modifier.PUBLIC, mth_addtimes.getModifiers());

	} catch(NoSuchMethodException e) {
	    fail("No such method");
	}
    }

    @Test
    public void test_value_initialized() {
	HostClass h = new HostClass();
	Class<HostClass> cls = HostClass.class;
	try{
	    Field fld_value = cls.getDeclaredField("value");
	    assertEquals(String.class, fld_value.getType());
	    assertEquals(Modifier.PRIVATE, fld_value.getModifiers());

	    fld_value.setAccessible(true);
	    Object val = fld_value.get(h);
	    assertNotNull(val);
	    assertEquals("hello ", val);
	} catch(NoSuchFieldException e) {
	    fail("No such field");
	} catch(IllegalAccessException e) {
	    fail("Illegal access exception");
	}
    }

}
