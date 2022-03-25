#include <cstdlib>
#include <cstring>

class N
{
private:
	char	annotation[100];
	int		value;

public:
	N(int value): value(value)
	{ }

	virtual int	operator+(N &other)
	{ return value + other.value; }

	virtual int	operator-(N &other)
	{ return value - other.value; }

	void setAnnotation(char *str)
	{ memcpy(annotation, str, strlen(str)); }
};

int main(int ac, char **av)
{
	N	*a;
	N	*b;

	if (ac < 2) {
		exit(1);
	}

	a = new N(5);
	b = new N(6);

	a->setAnnotation(av[1]);

	return *b + *a;
}
