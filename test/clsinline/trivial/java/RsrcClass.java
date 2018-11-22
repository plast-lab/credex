package clsinline.trivial;


class RsrcClass
{

    private String value;

    public RsrcClass(String _value) {
	value = _value;
    }

    
    public String addtimes(int n) {
	StringBuffer sb = new StringBuffer(value.length()*n);

	for(int i=0;i<n;i++)
	    sb.append(value);

	return sb.toString();
    }

}
