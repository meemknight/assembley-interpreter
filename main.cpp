#include <string>
#include <unordered_map>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <list>

inline bool isNum(char a)
{
	return (a <= '9' && a >= '0');
}

inline bool isNumMin(char a)
{
	return ((a <= '9' && a >= '0') || a == '-');
}

std::unordered_map<std::string, int> regs;
std::unordered_map<std::string, int> labels;

int compareFlag = 0;
// 0 equal
// 1 x > y 
// -1 x < y

std::list<int> callStack;
std::string output = "";

#define FUNCTIONS_SIZE 20
enum functions
{
	mov,
	inc,
	dec,
	jnz,
	add,
	sub,
	mul,
	division,
	jmp,
	cmp,
	jne,
	je,
	jg,
	jge,
	jl,
	jle,
	call,
	ret,
	msg,
	end,
};

char functionsName[FUNCTIONS_SIZE][5]
{
	"mov",
	"inc",
	"dec",
	"jnz",
	"add",
	"sub",
	"mul",
	"div",
	"jmp",
	"cmp",
	"jne",
	"je",
	"jg",
	"jge",
	"jl",
	"jle",
	"call",
	"ret",
	"msg",
	"end",
};

void makeComparison(int x, int y)
{
	if(x == y)
	{
		compareFlag = 0;
		return;
	}else 
	if(x > y)
	{
		compareFlag = 1;
		return;
	}else if(x < y)
	{
		compareFlag = -1;
		return;
	}
}


int getLabel(const std::vector<std::string>& program, int line, int &pos, char* labl = nullptr)
{
	while (program[line][pos] == ' ' || program[line][pos] == ',' || program[line][pos] == '\t')
	{
		pos++;
	}

	char temp[30];
	int size = 0;
	int endPos = pos;
	while (program[line][endPos] != ' ' && program[line][endPos] != '\t' && program[line][endPos] != '\n' && program[line][endPos] != '\0' && program[line][endPos] != ',')
	{
		temp[size] = program[line][endPos];
		size++;
		endPos++;
	}
	temp[size] = '\0';
	pos = endPos;

	//if(regs.find(temp) == regs.end())
	if(labl)
	{
		strcpy(labl, temp);
	}

	return labels[temp];

}


std::unordered_map<std::string, int>::iterator getReg(const std::vector<std::string>& program, int line, int &pos)
{
	while (program[line][pos] == ' ' || program[line][pos] == ',' || program[line][pos] == '\t')
	{
		pos++;
	}

	char temp[30];
	int size = 0;
	int endPos = pos;
	while (program[line][endPos] != ' ' && program[line][endPos] != '\t' && program[line][endPos] != '\n' && program[line][endPos] != '\0' && program[line][endPos] != ',')
	{
		temp[size] = program[line][endPos];
		size++;
		endPos++;
	}
	temp[size] = '\0';
	pos = endPos;

	if(regs.find(temp) == regs.end())
	{
		regs[temp] = 0;
	}

	return regs.find(temp);
}

int getRegOrNum(const std::vector<std::string>& program, int line, int &pos)
{
	while (program[line][pos] == ' ' || program[line][pos] == ',' || program[line][pos] == '\t' )
	{
		pos++;
	}

	if (!isalpha(program[line][pos]))
	{
		int number = 0;
		int neg = 1;
		while (program[line][pos] != ' ' && program[line][pos] != '\t' && program[line][pos] != '\n' && program[line][pos] != '\0' && program[line][pos] != ',')
		{
			if (program[line][pos] == '-')
			{
				neg = -1;
				pos++;
				continue;
			}
			number *= 10;
			number += program[line][pos] - '0';
			pos++;

		}
		number *= neg;
		return number;
	}
	else
	{
		return getReg(program, line, pos)->second;
	}

}

int readFunc(const std::vector<std::string>& program, int line, int &pos)
{
	char temp[30];
	int size = 0;
	int endPos = pos;
	while (program[line][endPos] != ' ' && program[line][endPos] != '\t' && program[line][endPos] != '\0' && program[line][endPos] != '\n')
	{
		temp[size] = program[line][endPos];
		size++;
		endPos++;
	}

	temp[size] = '\0';
	pos = endPos;
	if(size!=0)
	for (int i = 0; i< FUNCTIONS_SIZE; i++)
	{
		if (strstr(&functionsName[i][0], temp))
		{
			return i;
		}
	}


	return -1;
}

void parseMessage(const std::vector<std::string>& program, int line, int pos)
{
	int state = 0;
	// 0 = nothing
	// 1 = reading

	for( ;pos<program[line].length(); pos++)
	{
		if(program[line][pos] == ';' || program[line][pos] == '\n' || program[line][pos] == '\t')
		{
			break;
		}
	
		if(program[line][pos] == '\'')
		{
			if(state == 0)
			{
				state = 1;
			}else
			{
				state = 0;
			}
			continue;
		}

		if(state == 1)
		{
			output += program[line][pos];
		}
		else
		{
			if (program[line][pos] != ' ' && program[line][pos] != ',')
			{
				int a = getRegOrNum(program, line, pos);
				int size = 0;
				char copy[11];

				if(a == 0)
				{
					output += '0';
				}else
				{
					if(a < 0)
					{
						output += '-';
						a = -a;
					}

					while(a)
					{
						copy[size] = a%10 + '0';
						size++;
						a /= 10;
					}

					for(int i=size-1; i>=0; i--)
					{
						output += copy[i];
					}
				}

				
				
			}
		}

	}
}

std::string assembler_interpreter(std::string program)
{
	output = "";
	output.reserve(100);

	callStack.clear();
	regs.clear();
	labels.clear();

	std::vector<std::string> p;
	p.reserve(25);

	std::string temp = "";
	temp.reserve(100);
	int line = 0;
	bool isComment = 0;

	for(int i=0; i<program.size(); i++)
	{
		if(program[i] == ':')
		{
			labels[temp] = line;
			temp += program[i];
		}else
		if(program[i] == '\n')
		{
			isComment = 0;
			p.emplace_back(std::move(temp));
			temp = "";
			line++;
			continue;
		}else
		if(program[i] == ';')
		{
			isComment = 1;
		}else
		{
			if(!isComment)
			{
				if(program[i] != '\t')
				{
					temp += program[i];
				}
			}
		}


	}
	///////////////////////////////////

	if(temp != "")
	{
		p.emplace_back(std::move(temp));
		line++;
	}
	bool endedCorectly = 0;

	for (int line = 0; line < p.size(); line++)
	{
		int pos = 0;
		while (p[line][pos] == ' ')
		{
			pos++;
		}

		int function = readFunc(p, line, pos);


		if (function == mov)
		{
			auto first = getReg(p, line, pos);
			const int& second = getRegOrNum(p, line, pos);
			first->second = second;
		}
		else if (function == inc)
		{
			auto r = getReg(p, line, pos);
			r->second = r->second + 1;

		}
		else if (function == dec)
		{
			auto r = getReg(p, line, pos);
			r->second = r->second - 1;
		}
		else if (function == jnz) //depricated
		{
			int a = getRegOrNum(p, line, pos);
			if (a)
			{
				int jump = getRegOrNum(p, line, pos);
				if (jump)
				{
					line += jump;
					line -= 1;
				}

				continue;
			}
		}
		else if (function == add)
		{
			auto first = getReg(p, line, pos);
			const int& second = getRegOrNum(p, line, pos);
			first->second += second;
		}
		else if (function == sub)
		{
			auto first = getReg(p, line, pos);
			int second = getRegOrNum(p, line, pos);
			first->second -= second;
		}
		else if (function == mul)
		{
			auto first = getReg(p, line, pos);
			int second = getRegOrNum(p, line, pos);
			first->second *= second;
		}
		else if (function == division)
		{
			auto first = getReg(p, line, pos);
			int second = getRegOrNum(p, line, pos);
			first->second /= second;
		}
		else if (function == jmp)
		{
			int jump = getLabel(p, line, pos);

			line = jump;
			line -= 1;

			continue;
		}
		else if (function == cmp)
		{
			int x = getRegOrNum(p, line, pos);
			int y = getRegOrNum(p, line, pos);
			makeComparison(x, y);
		}
		else if (function == jne)
		{
			if (compareFlag != 0)
			{
				int jump = getLabel(p, line, pos);

				line = jump;
				line -= 1;

				continue;
			}
		}
		else if (function == je)
		{
			if (compareFlag == 0)
			{
				int jump = getLabel(p, line, pos);

				line = jump;
				line -= 1;

				continue;
			}
		}
		else if (function == jge)
		{
			if (compareFlag >= 0)
			{
				int jump = getLabel(p, line, pos);

				line = jump;
				line -= 1;

				continue;
			}
		}
		else if (function == jg)
		{
			if (compareFlag > 0)
			{
				int jump = getLabel(p, line, pos);

				line = jump;
				line -= 1;

				continue;
			}
		}
		else if (function == jle)
		{
			if (compareFlag <= 0)
			{
				int jump = getLabel(p, line, pos);

				line = jump;
				line -= 1;

				continue;
			}
		}
		else if (function == jl)
		{
			if (compareFlag < 0)
			{
				int jump = getLabel(p, line, pos);

				line = jump;
				line -= 1;

				continue;
			}
		}
		else if (function == call)
		{
			int jump = getLabel(p, line, pos);

			callStack.push_front(line + 1);
			
			line = jump;
			line -= 1;
			continue;
		}
		else if (function == ret)
		{
			line = callStack.front();
			callStack.pop_front();
			line -= 1;
			continue;
		}
		else if (function == msg)
		{
			parseMessage(p, line, pos);
		}
		else if (function == end)
		{
			endedCorectly = 1;
			break;
		}

	}

	if(!endedCorectly)
	{
		return "-1";
	}else
	{
		return output;
	}

}

int main()
{
	std::string program = R"(
mov   a, 5
mov   b, a
mov   c, a
call  proc_fact
call  print
end

proc_fact:
    dec   b
    mul   c, b
    cmp   b, 1
    jne   proc_fact
    ret

print:
    msg   a, '! = ', c ; output text
    ret
)";


	std::cout << assembler_interpreter
	(
	"mov rax, -3\n"
		"msg rax\n"
		"end"
	) << std::endl;

	std::cout << output << std::endl;

	//std::cout << regs["rax"] << '\n';
	//std::cout << compareFlag << '\n';

	std::cin.get();
	return 0;
}