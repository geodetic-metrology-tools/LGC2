#include <stdlib.h>
#include "QTextStream.h"

//////////////////////////////////////
// Stream manipulator class definition
//////////////////////////////////////
template<class T>
class QStreamManip
{  
public:
	QStreamManip(QTextStream& (*a)(QTextStream&, T), T v);
    friend QTextStream& operator<<(QTextStream&, const QStreamManip<T>&);
private:
	QTextStream& (*_action)(QTextStream&,T);
    T _value;
};

//////////////
// constructor
//////////////
template<class T>
QStreamManip< T >::QStreamManip(QTextStream& (*a)(QTextStream&,T), T v){

	_action = a;
	_value = v;
}


////////////////////////////
// isdigit() global function
////////////////////////////
bool isDigit(const char c){

	if ((c>='0') && (c<='9'))
		return true;
	else
		return false;
}



////////////////////////////////////////////
// overloaded QTextStream insertion operator
////////////////////////////////////////////
template<class T>
QTextStream& operator<<(QTextStream& s, const QStreamManip<T>& m){
	return (*m._action)(s, m._value);
}

///////////////////////////////
// do_setformat global function
///////////////////////////////
static QTextStream& do_setformat(QTextStream& os, const char fmt[]){
	int i = 0;
	while (fmt[i] != 0)
    {
		if (fmt[i] != '%'){
			os << fmt[i];
			i++;
		}
		else{
			i++;
			if (fmt[i] == '%'){
				os << fmt[i];
				i++;
			}
			else{
				bool ok = true;
                int istart = i;
                bool more = true;
                int width = 0;
                int precision = 6;
                int flags = 0;
                char fill = ' ';
                bool alternate = false;
                while (more){
					switch (fmt[i]){
						case '+':
							flags |= QTextStream::showpos;
                        break;
						case '-':
							flags |= QTextStream::left;
                        break;
						case '0':
							flags |= QTextStream::internal;
							fill = '0';
                        break;
						case '#':
							alternate = true;
                        break;
						case ' ':
                        break;
						default:
							more = false;
                        break;
                     }
                   if (more) i++;
				}
                if (isDigit(fmt[i])){
					width = atoi(fmt+i); 
					do i++; while (isDigit(fmt[i]));
				}
                 
                if (fmt[i] == '.'){
					i++;
                    precision = atoi(fmt+i); 
                    while (isDigit(fmt[i])) i++;
                }
                switch (fmt[i]){
					case 'd':
						flags |= QTextStream::dec;
                    break;
					case 'x':
						flags |= QTextStream::hex;
						if (alternate) flags |= QTextStream::showbase;
                    break;
					case 'X':
						flags |= QTextStream::hex | QTextStream::uppercase;
						if (alternate) flags |= QTextStream::showbase;
                    break;
					case 'o':
						flags |= QTextStream::hex;
						if (alternate) flags |= QTextStream::showbase;
                    break;
					case 'f':
						flags |= QTextStream::fixed;
						if (alternate) flags |= QTextStream::showpoint;
                    break;
					case 'e':
						flags |= QTextStream::scientific;
						if (alternate) flags |= QTextStream::showpoint;
                    break;
					case 'E':
						flags |= QTextStream::scientific | QTextStream::uppercase;
						if (alternate) flags |= QTextStream::showpoint;
                    break;
					case 'g':
						if (alternate) flags |= QTextStream::showpoint;
                    break;
					case 'G':
						flags |= QTextStream::uppercase;
						if (alternate) flags |= QTextStream::showpoint;
                    break;
					default:
						ok = false;
                    break;
				}
                
                i++;
                if (fmt[i] != 0) ok = false;
                if (ok){
					os.setf(flags);
                    os.width(width);
                    os.precision(precision);
                    os.fill(fill);
                }
                else
					i = istart;
			}
		}
		}
         
         return os;
}

///////////////////////////////////////
// global setformat function definition
///////////////////////////////////////
QStreamManip<const char*> setformat(const char* fmt){
	QStreamManip<const char*> ret(do_setformat, fmt);
	return ret;
}

