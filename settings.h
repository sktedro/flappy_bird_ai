/*
 * Toggles running the AI without drawing the game
 */
#define background 1

/*
 * Only weight sets that resulted in score higher or equal to the actual batch
 * number will be exported. If this is zero, 'minScoreToExport' will be used
 */
#define batchMeansLimit 1

/*
 * Or you can just set your limit manually - minimum score for the record to be
 * exported
 */
#define minScoreToExport 1

/* 
 * First barriers will not be randomly generated
 * this is to prevent them from getting generated in the same line
 */
#define firstBars 1 
  


/*
 * Print generated data by weight generator
 */
#define wgen_printData 0




/*
 *
 * DO NOT CHANGE IF YOU DON'T KNOW WHAT YOU'RE DOING
 *
 */

/*
 * Amount of weights to use. Changing this needs changes in (probably only) game.c
 */
#define weightsCount 4
