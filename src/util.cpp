#include <vector>
#include <string>
#include <cwctype>

namespace util
{
	class string
	{
	public:
		int split(const std::string& input, const std::string& seperator, std::vector<std::string>& con);
        std::string& trim(std::string &s);
	};


	int string::split(const std::string& input, const std::string& seperator, std::vector<std::string>& con)
	{
    	int len = seperator.size();
    	int pos = 0;
    	while(true)
    	{
    		int p = input.find(seperator, pos);
    		if(p < 0)
    		{
    			std::string s = input.substr(pos, input.size() - pos);
    			if(s.size())
    				con.push_back(s);

    			break;
    		}
    		std::string s = input.substr(pos, p - pos);
    		if(s.size())
				con.push_back(s);
    		pos = p + len;
    	}
    	return 0;
	}
    std::string& string::trim(std::string &s)
    {
        if(s.empty())
        {
            return s;
        }
        std::string::iterator iter;
        for(iter = s.begin(); iter != s.end(); iter++)
        {
            if(!iswspace(*iter))
                break;
        }
        s.erase(s.begin(), iter);
        for(iter = s.end() - 1; iswspace(*iter); iter--)
        {
            s.erase(iter);
        }
        return s;
    }
}