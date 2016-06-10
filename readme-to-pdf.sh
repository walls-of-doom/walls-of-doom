pandoc -N --template=template.tex\
    --variable mainfont="Liberation Sans"\
    --variable sansfont="Liberation Sans"\
    --variable monofont="Liberation Mono"\
    --variable fontsize=12pt\
    README.md --latex-engine=xelatex --toc -o README.pdf
