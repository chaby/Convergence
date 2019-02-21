AATree.o: AATree.c AATree.h
Alignment.o: Alignment.c Utils.h Alignment.h
AlignmentLikelihood.o: AlignmentLikelihood.c AlignmentLikelihood.h \
 Alignment.h Tree.h Utils.h EvolutionModel.h ColumnLikelihood.h \
 EvolutionModelProt.h EvolutionModelProtStored.h NLOpt.h
Annotation.o: Annotation.c Utils.h Annotation.h
Character.o: Character.c Character.h Utils.h
ColumnLikelihood.o: ColumnLikelihood.c ColumnLikelihood.h Tree.h Utils.h \
 EvolutionModel.h
ConvergenceExpectation.o: ConvergenceExpectation.c Utils.h \
 ConvergenceExpectation.h EvolutionModel.h Tree.h ColumnLikelihood.h \
 NLOpt.h EvolutionModelProtStored.h EvolutionModelProt.h
DensityUtils.o: DensityUtils.c DensityUtils.h
Enrichment.o: Enrichment.c Utils.h Annotation.h StatAnnotation.h
EvolutionModel.o: EvolutionModel.c EvolutionModel.h
EvolutionModelProt.o: EvolutionModelProt.c EvolutionModelProt.h \
 EvolutionModel.h
EvolutionModelProtStored.o: EvolutionModelProtStored.c \
 EvolutionModelProtStored.h Tree.h Utils.h EvolutionModel.h \
 EvolutionModelProt.h
MolSigDetect.o: MolSigDetect.c Tree.h Utils.h Alignment.h Character.h \
 EvolutionModel.h EvolutionModelProt.h EvolutionModelProtStored.h \
 ConvergenceExpectation.h ColumnLikelihood.h AlignmentLikelihood.h \
 NLOpt.h DensityUtils.h AATree.h
NLOpt.o: NLOpt.c NLOpt.h
StatAnnotation.o: StatAnnotation.c Utils.h StatAnnotation.h Annotation.h
Tree.o: Tree.c Tree.h Utils.h
Utils.o: Utils.c Utils.h
