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
        fld_value.setAccessible(true);
        Object value = fld_value.get(h);
        assertNotNull(value);
        assertEquals("hello ", value.toString());
       } catch(NoSuchFieldException e) {
        fail("No such field");
       } catch(IllegalAccessException e) {
        fail("Illegal access exception");
       }
    }

    @Test
    public void test_hasmeth() {
	   HostClass h = new HostClass();
    }

    @Test
    public void test_init_inlined() {
      HostClass h = new HostClass();
      Class<HostClass> cls = HostClass.class;
      try{
       Field fld_value = cls.getDeclaredField("value");
       assertEquals(String.class, fld_value.getType());
       fld_value.setAccessible(true);
       Object value = fld_value.get(h);
       assertNotNull(value);
       assertEquals("hello ", value.toString());
      } catch(NoSuchFieldException e) {
       fail("No such field");
      } catch(IllegalAccessException e) {
       fail("Illegal access exception");
      }
    }

}
