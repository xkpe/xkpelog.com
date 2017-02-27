
all: best-practices.html best-practices.pdf

best-practices.html: best-practices.md
	pandoc -s -o best-practices.html best-practices.md -c mdstyle2.css

best-practices.pdf: best-practices.md
	pandoc -s -o best-practices.pdf best-practices.md
