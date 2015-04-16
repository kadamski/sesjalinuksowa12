TEMPLATE=pandoc-templates/default.revealjs
PANDOC_OPTS=-t revealjs --template ${TEMPLATE} -V theme=white -V css=data/custom.css
FILENAME=slides

.PHONY: all clean

all: ${FILENAME}.html

${FILENAME}.html: ${FILENAME}.md
	pandoc ${PANDOC_OPTS} -s $< -o $@

clean:
	rm -f ${FILENAME}.html
