#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#define STUDENTID "102062801"

typedef struct CacheConfig
{
    int addressBus;
    int sets;
    int associativity;
    int offset;
} CacheConfig;

typedef struct ReferenceStringNode
{
    char str[33];
    struct ReferenceStringNode* next;
} ReferenceStringNode;

typedef struct ReferenceList
{
    ReferenceStringNode* head, *tail;
    int cnt;
} ReferenceList;

typedef struct ReferenceContent
{
    char header[256];
    char footer[256];
    unsigned* refs;
    char* isHits;
    int cnt;
} ReferenceContent;

typedef struct ProgramContext
{
    char orgFile[1024];
    char referenceFile[1024];
    char outputFile[1024];

    CacheConfig cacheConfig;
    ReferenceContent refContent;
    unsigned indexingBitCount;
    unsigned* indexingBits;
    unsigned indexingBitMask;
    void (*cache_fnc) (const CacheConfig* cfg, const unsigned* refs, char* ishits, int cnt, unsigned* indexingBitMask, unsigned* indexingBitCnt);
} ProgramContext;



static void program_init (ProgramContext* ctx, int argc, char** argv);
static void program_finalize (ProgramContext* ctx);
static void* __malloc_debug (size_t size, const char* file, int line, int init);
static ReferenceStringNode* ref_str_new_from (const char* str);
static void reflist_to_array (ReferenceList* list, unsigned** arr, int* cnt);
static void program_parse_cache_config (ProgramContext* ctx);
static void program_parse_reflist (ProgramContext* ctx);
static void cache_conf_print_stream (const CacheConfig* cache, FILE* stream);
static char* utobs (unsigned u, char* buf, int len, int addrBus );
static void ref_content_print_stream (ReferenceContent* content, FILE* stream, int addrBus);



static void* __malloc_debug (size_t size, const char* file, int line, int init)
{
    void* ret;
    ret = malloc (size);
    if (!ret)
    {
        fprintf (stderr, "at FILE %s LINE %d malloc(%d) failed:%s\n", file, line, (int) size, strerror (errno) );
        exit (errno);
        return NULL;
    }
    if (init) memset (ret, 0, size);
    return ret;
}
#define MALLOC(S) __malloc_debug((S),__FILE__,__LINE__,0)
#define ZALLOC(S) __malloc_debug((S),__FILE__,__LINE__,1)

static char* __myfgets (char* buf, size_t len, FILE* fp)
{
    char* ret = buf;
    char c;
    if (!fp) return NULL;
    if (feof (fp) )
    {
        return NULL;
    }
    while (len > 0)
    {
        c = fgetc (fp);
        if (c == '\n' || feof (fp) ) break;
        *buf = c;
        ++buf;
        --len;
    }
    *buf = '\0';
    return ret;
}

//////////////////////////////////////
// (parse-stage) create reference string
static ReferenceStringNode* ref_str_new_from (const char* str)
{
    ReferenceStringNode* ret = (ReferenceStringNode*) MALLOC (sizeof (ReferenceStringNode) );
    strncpy (ret->str, str, 33);
    return ret;
}

///////////////////////////////////
// (parse-stage) add string to reference list
static void reflist_add (ReferenceList* list, const char* s)
{
    if (!list->head)
    {
        list->tail = list->head = ref_str_new_from (s);
    }
    else
    {
        list->tail->next = ref_str_new_from (s);
        list->tail = list->tail->next;
    }
    ++list->cnt;
}

////////////////////////////////////////////////////////
// (parse-stage) convert reference string list to an array with unsigned type
static void reflist_to_array (ReferenceList* list, unsigned** arr, int* cnt)
{
    unsigned* ret = (unsigned*) MALLOC (sizeof (unsigned) * list->cnt);
    int idx = 0;
    ReferenceStringNode* i, *p;
    i = list->head;
    while (i != NULL)
    {
        ret[idx] = strtoul (i->str, NULL, 2);
        p = i;
        i = i->next;
        ++idx;
        free (p);
    }
    *cnt = list->cnt;
    *arr = ret;
}

//////////////////////////////////////////////////////
// initialize program, prepare program context 
// prepare file name from arguments.
static void program_init (ProgramContext* ctx, int argc, char** argv)
{
    if (argc < 3)
    {
        fprintf (stderr, "usage: %s [cacheFile] [referenceFile]\n", argv[0]);
        exit (-1);
        return;
    }
    memset (ctx, 0, sizeof (ProgramContext) );
    strncpy (ctx->orgFile, argv[1], 1024);
    strncpy (ctx->referenceFile, argv[2], 1024);
    if(argc > 3){ 
       strncpy(ctx->outputFile,argv[3],1024);
    }
    else{
       strncpy(ctx->outputFile,"index.rpt",1024);
    }
}

/////////////////////////////////////////////////////
// (parse-stage)  parse cache.org file into CacheConfig
static void program_parse_cache_config (ProgramContext* ctx)
{
    FILE* fpOrg = fopen (ctx->orgFile, "r");
    char linebuf[1024];
    char dummy[1024];
    if (!fpOrg)
    {
        perror (ctx->orgFile);
        exit (-1);
    }
    while (__myfgets (linebuf, 1024, fpOrg) )
    {
        if (strstr (linebuf, "Addressing_Bus") )
        {
            sscanf (linebuf, "%s : %d", dummy, &ctx->cacheConfig.addressBus);
        }
        if (strstr (linebuf, "Sets") )
        {
            sscanf (linebuf, "%s : %d", dummy, &ctx->cacheConfig.sets);
        }
        if (strstr (linebuf, "Associativity") )
        {
            sscanf (linebuf, "%s : %d", dummy, &ctx->cacheConfig.associativity);
        }
        if (strstr (linebuf, "Offset") )
        {
            sscanf (linebuf, "%s : %d", dummy, &ctx->cacheConfig.offset);
        }
    }
    fclose (fpOrg);
}

//////////////////////////////////////
// (finalize-stage) print cache configurations
static void cache_conf_print_stream (const CacheConfig* cache, FILE* stream)
{
    fprintf (stream, "Student ID: %s\n", STUDENTID);
    fprintf (stream, "Addressing Bus: %d\n", cache->addressBus);
    fprintf (stream, "Sets: %d\n", cache->sets);
    fprintf (stream, "Associativity: %d\n", cache->associativity);
    fprintf (stream, "Offset: %d\n",cache->offset);
}

//////////////////////////////////////
// convert unsigned integer to bits string
static char* utobs (unsigned u, char* buf, int len, int addrBus )
{
    unsigned mask = 1 << (addrBus - 1);
    char* ret = buf;
    while (len > 0 && addrBus > 0)
    {
        *buf = ( (u & mask) != 0) ? '1' : '0';
        ++buf;
        --len;
        --addrBus;
        mask >>= 1;
    }
    *buf = '\0';
    return ret;
}

//////////////////////////////////////////
// (finalize-stage) print reference content
// include header and footer
static void ref_content_print_stream (ReferenceContent* content, FILE* stream, int addrBus)
{
    char buf[32];
    int i;
    char* pbuf;
    fprintf (stream, "%s\n", content->header);
    for (i = 0; i < content->cnt; ++i)
    {
        pbuf = utobs (content->refs[i], buf, 32, addrBus);
        fprintf (stream, "%s %s\n", pbuf, (content->isHits[i] ? "hit" : "miss") );
    }
    fprintf (stream, "%s\n", content->footer);

}

//////////////////////////////////////////////////////////////
// (parse-stage) parse reference file(*.lst) into an array 
static void program_parse_reflist (ProgramContext* ctx)
{
    FILE* fpRef = fopen (ctx->referenceFile, "r");
    char linebuf[1024];
    ReferenceList* list;
    if (!fpRef)
    {
        perror (ctx->referenceFile);
        exit (-1);
    }
    list = (ReferenceList*) ZALLOC (sizeof (ReferenceList) );

    while (__myfgets (linebuf, 1024, fpRef) )
    {
        if (strstr (linebuf, ".benchmark ") )
        {
            strncpy (ctx->refContent.header, linebuf, 256);
            continue;
        }
        if (strstr (linebuf, ".end") )
        {
            strncpy (ctx->refContent.footer, linebuf, 256);
            break;
        }
        reflist_add (list, linebuf);
    }
    reflist_to_array (list, &ctx->refContent.refs, &ctx->refContent.cnt);
    ctx->refContent.isHits = (char*) ZALLOC (sizeof (char) * ctx->refContent.cnt);
    free (list);
    fclose (fpRef);
}

///////////////////////////////////////////////////
// (finalize-stage) print indexing bits count and indexing bits
// calculate total miss and print it 
static void program_print_indexing_and_cache_miss (ProgramContext* ctx, FILE* stream)
{
    int i;
    int miss = 0;
    fprintf (stream, "Indexing bits count: %d\n", ctx->indexingBitCount);
    fprintf (stream, "Indexing bits: ");
    for (i = ctx->cacheConfig.addressBus; i >= 0; --i)
    {
        if ( ctx->indexingBitMask & (1 << i) )
        {
            fprintf (stream, "%d ", i);
        }
    }
    fprintf (stream, "\n");
    for (i = 0; i < ctx->refContent.cnt; ++i)
    {
        if (ctx->refContent.isHits[i] == 0)
        {
            ++miss;
        }
    }
    fprintf (stream, "Total cache miss: %d\n\n\n\n", miss);
}

//////////////////////////////////////////////////
/// finalize stage
static void program_finalize (ProgramContext* ctx)
{
    FILE* fp = fopen(ctx->outputFile,"w");
    cache_conf_print_stream (&ctx->cacheConfig, fp);
    program_print_indexing_and_cache_miss (ctx, fp);
    ref_content_print_stream (&ctx->refContent, fp, ctx->cacheConfig.addressBus);
    free (ctx->refContent.refs);
    free (ctx->refContent.isHits);
    fclose(fp);
}

//////////////////////////////////////////////
/// running stage  
static void program_run_cache_fnc (ProgramContext* ctx)
{
    if (ctx->cache_fnc)
    {
        ctx->cache_fnc (&ctx->cacheConfig, ctx->refContent.refs, ctx->refContent.isHits, ctx->refContent.cnt, &ctx->indexingBitMask, &ctx->indexingBitCount);
    }
}

/////////////////////////////////////////////////////////////////////////////////////
static void zero_cost_step1 (unsigned addressBus, const unsigned* refs, int cnt, double Q[32])
{
    unsigned int maskidx = 0;
    unsigned int mask = 1;
    for (maskidx = 0; maskidx < addressBus; ++maskidx)
    {
        int i;
        int sum[2] = {0, 0};
        for (i = 0; i < cnt; ++i)
        {
            if ( (refs[ i ]&mask) != 0)
            {
                sum[1] += 1;
            }
            else
            {
                sum[0] += 1;
            }
        }
        if (sum[0] > sum[1])
        {
            Q[maskidx] = ( (double) sum[1]) / sum[0];
        }
        else
        {
            Q[maskidx] = ( (double) sum[0]) / sum[1];
        }
        mask = mask << 1;
    }
}

static void zero_cost_step2 (unsigned addressBus, const unsigned* refs, int cnt, double C[1024])
{
    int i, j, k;
    for (i = 0; i < addressBus; ++i)
    {
        unsigned mask1 = 1 << i;
        for (j = 0; j < addressBus; ++j)
        {
            unsigned mask2 = 1 << j;
            int sum[2] = {0, 0};
            if (i == j)
            {
                C[ i * addressBus + j ] = 0;
                continue;
            }
            // for each k, calculate Ci,j
            for (k = 0; k < cnt; ++k)
            {
                // mask and get its bit
                unsigned res1 = !! (refs[k] & mask1);
                unsigned res2 = !! (refs[k] & mask2);
                sum[ (res1 != res2)] += 1;
            }
            if (sum[1] > sum[0])
            {
                C[ i * addressBus + j ] = ( (double) sum[0]) / sum[1];
            }
            else
            {
                C[ i * addressBus + j ] = ( (double) sum[1]) / sum[0];
            }
        }
    }
}

static void showQ (unsigned addressBus, double Q[32]);
static void showC (unsigned addressBus, double C[1024]);
static void showbits (const char* name, unsigned M, unsigned S);

static int zero_cost_step3_A_is_in_S (unsigned S, unsigned A)
{
    return !! (S & (1 << A) );
}

static int zero_cost_step3_maxQ (unsigned M, double Q[32], unsigned S)
{
    int i;
    int ret = 0;
    for (i = 0; i < M; ++i)
    {
        if (zero_cost_step3_A_is_in_S (S, i) ) continue; // skip if it is in set.
        if (Q[i] > Q[ret])
        {
            ret = i;
        }
    }
    return ret;
}

static void showbits (const char* name, unsigned M, unsigned S)
{
    char buf[32];
    char*  pbuf = utobs (S, buf, 32, M);
    fprintf (stderr, "%s=%s \n", name, pbuf);
}

static void showQ (unsigned addressBus, double Q[32])
{
    int i;
    for (i = 0; i < addressBus; ++i)
    {
        fprintf (stderr, "Q[%d]=%f\n", i, Q[i]);
    }
}

static void showC (unsigned addressBus, double C[1024])
{
    int i, j;
    fprintf (stderr, "C=\n");
    for (i = 0; i < addressBus; ++i)
    {
        for (j = 0; j < addressBus; ++j)
        {
            fprintf (stderr, "%-3.2f ", C[j * addressBus + i]);
        }
        fprintf (stderr, "\n");
    }
}

static unsigned zero_cost_step3 (unsigned M, double Q[32], double C[1024], unsigned S, int bitsCnt)
{
    int i, j;
    double maxQ;
    unsigned Abest = 0;
    double Q2[32];
    memcpy (Q2, Q, sizeof (double) * 32);
    for (i = 0; i < M && i < bitsCnt; ++i)
    {
        Abest = zero_cost_step3_maxQ (M, Q2, S);
        S |= (1 << Abest);
        for (j = 0; j < M; ++j)
        {
            Q2[j] = Q2[j] * C[Abest * M + j];
        }
    }
    return S;
}

static unsigned getIndexValue (unsigned index, unsigned* indexbits, unsigned indexLen)
{
    int i;
    unsigned ret = 0;
    for (i = 0; i < indexLen; ++i)
    {
        if ( (index & (1 << indexbits[i]) ) != 0)
        {
            ret += (1 << (indexLen - i - 1) );
        }
    }
    return ret;
}

typedef struct LinkedListNode
{
    unsigned ref;
    struct LinkedListNode* next, *prev;
} LinkedListNode;

typedef struct ListSet
{
    LinkedListNode* head;
    LinkedListNode* tail;
    int cnt;
    int max;
} ListSet;

static LinkedListNode* create_ref_node (unsigned ref)
{
    LinkedListNode* ret = (LinkedListNode*) ZALLOC (sizeof (LinkedListNode) );
    ret->ref = ref;
    return ret;
}

static ListSet* create_list_set (int associativity)
{
    ListSet* ret = (ListSet*) ZALLOC (sizeof (ListSet) );
    ret->max = associativity;
    return ret;
}

static void delete_list_set (ListSet* set)
{
    LinkedListNode* n, *i;
    if (!set) return;
    i = set->head;
    while (i != NULL)
    {
        n = i;
        i = i->next;
        free (n);
    }
    free (set);
}

static void listset_move_to_front (ListSet* set, LinkedListNode* node)
{
    if (node->next)
    {
        node->next->prev = node->prev;
    }
    else   // node is tail
    {
        if(node->prev)
           set->tail = node->prev;
    }
    if (node->prev)
    {
        node->prev->next = node->next;
        node->prev = NULL;
        if (set->head)
        {
            set->head->prev = node;
        }
        node->next = set->head;
        set->head = node;
    }
    else
    {
        // it's already the front one.
        return;
    }
}

static int access_listset_lru (ListSet* set, unsigned ref)
{
    int i;
    if (set->cnt == 0)
    {
        set->head = set->tail = create_ref_node (ref);
        set->cnt = 1;
        return 0;
    }
    else
    {
        LinkedListNode* iter;
        for (iter = set->head; iter != NULL; iter = iter->next)
        {
            if (iter->ref == ref)
            {
                listset_move_to_front (set, iter);
                return 1;
            }
        }
    }
    /// not found
    if (set->cnt < set->max)
    {
        LinkedListNode* newnode = create_ref_node (ref);
        newnode->next = set->head;
        set->head->prev = newnode;
        set->head = newnode;
        ++set->cnt;
    }
    else
    {
        LinkedListNode* newnode = set->tail;
        if (newnode == NULL)
        {
            fprintf (stderr, "%s %d %s...newnode=null, cnt=%d\n", __FILE__, __LINE__, __func__, set->cnt);
            return 0;
        }
        newnode->ref = ref;
        listset_move_to_front (set, newnode);
    }
    return 0;
}

typedef struct TwoWaySet
{
    unsigned ref[2];
    unsigned cnt;
} TwoWaySet;

static int access_set_lru2way (TwoWaySet* set, unsigned ref)
{
    int i;
    unsigned* refs = set->ref;
    for (i = 0; i < set->cnt; ++i)
    {
        if (refs[i] == ref)
        {
            if (i != 0)  // swap
            {
                unsigned t = refs[0];
                refs[0] = ref;
                refs[1] = t;
            }
            return 1;
        }
    }
    /// not found
    if (set->cnt == 0)
    {
        refs[0] = ref;
        set->cnt = 1;
    }
    else
    {
        // if it is full, the bottom one is replaced with ref
        // the top one is pushed to bottom, and then ref becomes top
        unsigned t = refs[0];
        refs[0] = ref;
        refs[1] = t;
        set->cnt = 2;
    }
    return 0;
}

#define OFFSET_MASK(REF,MASK) ((REF)&(~(MASK)))
//#define OFFSET_MASK(REF,MASK) (REF)
static void twoWayAssociate (unsigned offsetMask,const unsigned* refs, char* ishits, int cnt,
                             unsigned indexingBitMask, unsigned* indexbits, unsigned indexLen, unsigned setCnt)
{
    TwoWaySet* sets = (TwoWaySet*) ZALLOC (sizeof (TwoWaySet) * setCnt);
    int i;
    for (i = 0; i < cnt; ++i)
    {
        unsigned ref = refs[i];
        unsigned index = ref & indexingBitMask;
        unsigned indexValue = getIndexValue (index, indexbits, indexLen);
        ishits[i] = access_set_lru2way (&sets[indexValue], OFFSET_MASK(ref,offsetMask));
    }
    free (sets);
}

static int access_set_directmap (char* isset, unsigned* sets, unsigned ref, unsigned indexValue)
{
    if (!isset[indexValue])
    {
        sets[indexValue] = ref;
        isset[indexValue] = 1;
        return 0;
    }
    if (sets[indexValue] == ref) return 1;
    sets[indexValue] = ref;
    return 0;
}

static void directmapAssociate (unsigned offsetMask,const unsigned* refs, char* ishits, int cnt,
                                unsigned indexingBitMask, unsigned* indexbits, unsigned indexLen, unsigned setCnt)
{
    int i;
    char* isset = (char*) ZALLOC (sizeof (char) * setCnt);
    unsigned* sets = (unsigned*) MALLOC (sizeof (unsigned) * setCnt);
    for (i = 0; i < cnt; ++i)
    {
        unsigned ref = refs[i];
        unsigned index = ref & indexingBitMask;
        unsigned indexValue = getIndexValue (index, indexbits, indexLen);
        ishits[i] = access_set_directmap (isset, sets, OFFSET_MASK(ref,offsetMask), indexValue);
    }
    free (isset);
    free (sets);
}

static void runAssociate (int associativity,int offsetMask, const unsigned* refs, char* ishits, int cnt,
                          unsigned indexingBitMask, unsigned* indexbits, unsigned indexLen, unsigned setCnt)
{
    if (associativity == 1)
    {
        // direct map
        directmapAssociate (offsetMask,refs, ishits, cnt, indexingBitMask, indexbits, indexLen, setCnt);
        return;
    }
    if (associativity == 2)
    {
        twoWayAssociate (offsetMask,refs, ishits, cnt, indexingBitMask, indexbits, indexLen, setCnt);
        return;
    }
    else
    {
        ListSet** sets = (ListSet**) ZALLOC (sizeof (ListSet*) * setCnt);
        int i;
        for (i = 0; i < setCnt; ++i)
        {
            sets[i] = create_list_set (associativity);
        }
        for (i = 0; i < cnt; ++i)
        {
            unsigned ref = refs[i];
            unsigned index = ref & indexingBitMask;
            unsigned indexValue = getIndexValue (index, indexbits, indexLen);
            ishits[i] = access_listset_lru (sets[indexValue],OFFSET_MASK(ref,offsetMask));
        }
        for (i = 0; i < setCnt; ++i)
        {
            delete_list_set (sets[i]);
        }
        free (sets);
    }
}

///////////////////////////////////////////////////////////////////////
/// run  zero-cost algorithm (zero_cost_step1, zero_cost_step2, zero_cost_step3)
/// use the zero-cost algorithm finding indexing bits
/// use indexing bits to invoke runAssociate
static void cache_fnc (const CacheConfig* cfg,
                       const unsigned* refs, char* ishits, int cnt,
                       unsigned* indexingBitMask, unsigned* indexBitCnt)
{
    /////////////////////////////////////////////
    // Zero cost Algorithm implementation
    unsigned indexLength = 0;
    unsigned addressBus = cfg->addressBus;
    unsigned offsetMask = ~ ( ( (~0) >> (cfg->offset) ) << (cfg->offset) );
    unsigned addressBusMask = ~ ( ( (~0) >> addressBus) << addressBus);
    unsigned S = 0;
    int i, j;
    double Q[32];
    double C[1024];
    unsigned indexingBits[32];
    memset (Q, 0, sizeof (double) * 32);
    memset (C, 0, sizeof (double) * 1024);
    indexLength = (unsigned) log2 ( (double) cfg->sets);
    zero_cost_step1 (addressBus, refs, cnt, Q);//算Q 
    zero_cost_step2 (addressBus, refs, cnt, C);//算C 
    S |= offsetMask; // use to guard algorithm from offset bits.
    S = zero_cost_step3 (addressBus, Q, C, S, indexLength);
    S &= (~offsetMask); // eliminate offset bits.
    *indexBitCnt = indexLength;
    *indexingBitMask = S;
    ////////////////////////////////////////////
    j = 0;
    for (i = cfg->addressBus; i >= 0; --i)
    {
        if ( S & (1 << i) )
        {
            indexingBits[j] = i;
            ++j;
        }
    }
    runAssociate (cfg->associativity,offsetMask, refs, ishits, cnt, S, indexingBits, indexLength, cfg->sets);
}

int main (int argc, char** argv)
{
    ProgramContext ctx;
    
    program_init (&ctx, argc, argv);//會初始化ProgramContext結構，將執行參數放進去
    
    program_parse_cache_config (&ctx);//(解析用)會讀輸入的cache設定檔，解出設定值
    program_parse_reflist (&ctx);//(解析用)讀取並解析reference list檔，就是.lst的檔案
    
    ctx.cache_fnc = cache_fnc;//zoro cost algorithm 
    program_run_cache_fnc (&ctx);//會啟動執行的函式，會去叫用ctx.cache_fnc的函式
    
    program_finalize (&ctx);//顯示統計內容
}
