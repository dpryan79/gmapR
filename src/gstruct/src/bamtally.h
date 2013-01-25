/* $Id: bamtally.h 67869 2012-07-02 21:39:26Z twu $ */
#ifndef BAMTALLY_INCLUDED
#define BAMTALLY_INCLUDED
#include "bamread.h"
#include "genome.h"
#include "genomicpos.h"
#include "list.h"
#include "iit-read.h"
#include "tableuint.h"


typedef enum {OUTPUT_BLOCKS, OUTPUT_RUNLENGTHS, OUTPUT_TALLY, OUTPUT_IIT} Tally_outputtype_T;
#define DEFAULT_QUALITY 40  /* quality_score_adj + 40 */

extern void
Bamtally_run (long int **tally_matches, long int **tally_mismatches,
	      List_T *intervallist, List_T *labellist, List_T *datalist,
	      int *quality_counts_match, int *quality_counts_mismatch,
	      Bamreader_T bamreader, Genome_T genome, char *chr,
	      Genomicpos_T chroffset, Genomicpos_T chrstart, Genomicpos_T chrend, int alloclength,
	      Tableuint_T resolve_low_table, Tableuint_T resolve_high_table,
	      int minimum_mapq, int good_unique_mapq, int maximum_nhits,
	      bool need_concordant_p, bool need_unique_p, bool need_primary_p,
	      bool ignore_lowend_p, bool ignore_highend_p,
	      Tally_outputtype_T output_type, bool blockp, int blocksize,
	      int quality_score_adj, int min_depth, int variant_strands,
	      bool genomic_diff_p, bool signed_counts_p, bool ignore_query_Ns_p,
	      bool print_indels_p, bool print_totals_p, bool print_cycles_p,
	      bool print_quality_scores_p, bool print_mapq_scores_p, bool want_genotypes_p,
	      bool verbosep);

extern void
Bamtally_run_lh (long int **tally_matches_low, long int **tally_mismatches_low,
		 long int **tally_matches_high, long int **tally_mismatches_high,
		 int *quality_counts_match, int *quality_counts_mismatch,
		 Bamreader_T bamreader, Genome_T genome, char *chr,
		 Genomicpos_T chroffset, Genomicpos_T chrstart, Genomicpos_T chrend, int alloclength,
		 int minimum_mapq, int good_unique_mapq, int maximum_nhits,
		 bool need_concordant_p, bool need_unique_p, bool need_primary_p,
		 int blocksize, int quality_score_adj, int min_depth, int variant_strands,
		 bool genomic_diff_p, bool ignore_query_Ns_p, bool verbosep);

extern IIT_T
Bamtally_iit (Bamreader_T bamreader, char *chr, Genomicpos_T chrstart, Genomicpos_T chrend,
	      Genome_T genome, IIT_T chromosome_iit, int alloclength,
	      int minimum_mapq, int good_unique_mapq, int maximum_nhits,
	      bool need_concordant_p, bool need_unique_p, bool need_primary_p,
	      int min_depth, int variant_strands, bool ignore_query_Ns_p,
	      bool print_indels_p, int blocksize, bool verbosep);

#endif