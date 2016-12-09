Here you can find structures and layouts of 18S rRNA for species from the metazoa kingdom downloaded from the  [Comparative RNA Web (CRW) Site](http://www.rna.icmb.utexas.edu/). CRW is a repository storing visualizations of ribosomal RNAs in the form as used by the biological community. In order to lay out, for example, human rRNA using mouse rRNA as the template, run:

```shell
PATH_TO_TRAVELER\traveler \
    --target-structure human.fasta \
    --template-structure mous.ps mouse.fasta \
    --all human_from_mouse
```
