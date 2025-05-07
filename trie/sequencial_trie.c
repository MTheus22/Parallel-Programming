#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h> // Incluir para medição de tempo

// Tamanho do alfabeto para letras minúsculas 'a'-'z'
#define ALPHABET_SIZE 26

// Estrutura para um nó da Trie
struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    // true se o nó representa o final de uma palavra
    bool isEndOfWord;
};

// Função para criar um novo nó da Trie
struct TrieNode *createNode() {
    struct TrieNode *newNode = (struct TrieNode *)malloc(sizeof(struct TrieNode));

    if (newNode) {
        int i;
        newNode->isEndOfWord = false; // Inicializa como não sendo fim de palavra

        // Inicializa todos os ponteiros filhos como NULL
        for (i = 0; i < ALPHABET_SIZE; i++) {
            newNode->children[i] = NULL;
        }
    }
    return newNode;
}

// Função para inserir uma string na Trie
void insert(struct TrieNode *root, const char *key) {
    struct TrieNode *currentNode = root;
    int length = strlen(key);
    int i;

    for (i = 0; i < length; i++) {
        // Calcula o índice do caractere (ex: 'a' -> 0, 'b' -> 1)
        int index = key[i] - 'a';

        // Verifica se o caractere está dentro do alfabeto esperado
        if (index < 0 || index >= ALPHABET_SIZE) {
            // printf("Erro na inserção: Caractere '%c' fora do alfabeto suportado ('a'-'z').\n", key[i]);
            return; // Ignora caracteres inválidos silenciosamente para não afetar a medição de tempo principal
        }

        // Se o filho correspondente ao caractere não existe, cria um novo nó
        if (!currentNode->children[index]) {
            currentNode->children[index] = createNode();
        }

        // Move para o nó filho
        currentNode = currentNode->children[index];
    }

    // Marca o nó final como o fim de uma palavra
    currentNode->isEndOfWord = true;
}

// Função para buscar uma string na Trie
// Retorna true se a string for encontrada como uma palavra completa, false caso contrário
bool search(struct TrieNode *root, const char *key) {
    struct TrieNode *currentNode = root;
    int length = strlen(key);
    int i;

    for (i = 0; i < length; i++) {
        int index = key[i] - 'a';

         // Verifica se o caractere está dentro do alfabeto esperado
        if (index < 0 || index >= ALPHABET_SIZE) {
            return false; // Caractere inválido, palavra não está na Trie
        }

        // Se o filho correspondente ao caractere não existe, a palavra não está na Trie
        if (!currentNode->children[index]) {
            return false;
        }

        // Move para o nó filho
        currentNode = currentNode->children[index];
    }

    // Se chegamos ao final da string e o nó atual está marcado como fim de palavra,
    // a string foi encontrada.
    return (currentNode != NULL && currentNode->isEndOfWord);
}

// Função auxiliar para liberar a memória alocada pela Trie (recursivo)
void freeTrie(struct TrieNode *node) {
    if (!node) {
        return;
    }

    int i;
    for (i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            freeTrie(node->children[i]); // Libera recursivamente os filhos
        }
    }

    free(node); // Libera o nó atual
}


// --- Exemplo de Uso com Medição de Tempo ---
int main() {
    // Lista de palavras para inserir
    char words[][32] = {"the", "a", "there", "answer", "any", "by", "bye", "their",
                        "theirself", "them", "themselves", "then", "thence", "hence",
                        "thereafter", "thereby", "therefore", "thereto", "thermos",
                        "these", "they", "thick", "thin", "thing", "think", "third",
                        "this", "those", "though", "thought", "thousand", "three",
                        "thro", "through", "throughout", "throw", "thru", "thus",
                        "thy", "thyself", "tie", "tight", "till", "time", "tin",
                        "tip", "tire", "to", "today", "together", "told", "ton",
                        "tone", "tongue", "tonight", "too", "took", "tool", "top",
                        "topsy", "turvy", "toss", "tot", "touch", "tough", "tour",
                        "tow", "toward", "towards", "tower", "town", "toy", "trace",
                        "track", "trade", "train", "tramp", "transfer", "trap", "trash",
                        "travel", "traverse", "tray", "tread", "treasure", "treat",
                        "treaty", "tree", "tremble", "trick", "trim", "trip", "troop",
                        "trot", "trouble", "trough", "trousers", "trout", "trow", "truce",
                        "true", "truly", "trump", "trunk", "trust", "truth", "try",
                        "tub", "tube", "tumble", "tune", "turn", "tutor", "twain",
                        "tweed", "twelfth", "twelve", "twentieth", "twenty", "twice",
                        "twig", "twilight", "twin", "twine", "twinkle", "twirl", "twist",
                        "twit", "two", "tying", "type", "typo", "ugly", "ulcer",
                        "ultimate", "ultimo", "ultra", "umbrella", "un", "unanimous",
                        "uncanny", "uncertain", "uncle", "uncommon", "unconscious",
                        "under", "undergo", "underground", "underneath", "undersigned",
                        "understand", "undertake", "undertaking", "underwear", "undetermined",
                        "undoubtedly", "unequal", "uneven", "unexpected", "unfair",
                        "unfit", "unfold", "unfortunate", "unfortunately", "unfrequented",
                        "unfriendly", "unfurnished", "ungainly", "unheard", "unholy",
                        "uniform", "unite", "united", "units", "unity", "universal",
                        "university", "unkind", "unknown", "unlawful", "unless", "unlike",
                        "unlikely", "unloading", "unlucky", "unmarried", "unmerciful",
                        "unmistakable", "unnecessary", "unoccupied", "unpaid", "unparalleled",
                        "unpleasant", "unprecedented", "unprejudiced", "unprepared",
                        "unprincipled", "unprotected", "unprovided", "unpublished",
                        "unpunished", "unqualified", "unquestionable", "unravel",
                        "unreasonable", "unredeemed", "unregulated", "unreliable",
                        "unremitting", "unreserved", "unrestrained", "unrestricted",
                        "unrighteous", "unrivaled", "unroll", "unruffled", "unsatisfactory",
                        "unseasonable", "unseen", "unselfish", "unsettled", "unshaken",
                        "unsightly", "unskillful", "unsociable", "unsolicited",
                        "unsophisticated", "unspeakable", "unstable", "unsteady",
                        "unsuccessful", "unsuitable", "unsurpassed", "unsuspecting",
                        "untamed", "untimely", "untiring", "untold", "untouched",
                        "untoward", "untried", "untrue", "unusual", "unvarnished",
                        "unveil", "unwarrantable", "unwary", "unwearied", "unwelcome",
                        "unwell", "unwieldy", "unwilling", "unwind", "unwise",
                        "unwittingly", "unwomanly", "unwonted", "unworthy", "unwrap",
                        "unyoke", "up", "upbraid", "upheld", "uphill", "uphold",
                        "upland", "upon", "upper", "uppermost", "upright", "uprising",
                        "uproar", "uproot", "upset", "upstairs", "upstream", "upward",
                        "urban", "urchin", "urge", "urgent", "usage", "use",
                        "useful", "usefulness", "useless", "usher", "usual", "usurp",
                        "usury", "utensil", "utility", "utilize", "utmost", "utter",
                        "utterance", "utterly", "v", "vacancy", "vacant", "vacate",
                        "vacation", "vague", "vaguely", "vain", "valentine", "valet",
                        "valiant", "valid", "validity", "valley", "valor", "valuable",
                        "valuation", "value", "valve", "vampire", "van", "vandal",
                        "vane", "vang", "vanilla", "vanish", "vanity", "vanquish",
                        "vantage", "vapid", "vapor", "variable", "variation", "varied",
                        "variety", "various", "varnish", "vary", "vase", "vassal",
                        "vast", "vastly", "vat", "vault", "vaunt", "vicious", "victim",
                        "victor", "victorious", "victory", "victual", "vie", "view",
                        "vigil", "vigilant", "vigor", "vigorous", "vile", "village",
                        "villain", "vine", "vinegar", "vineyard", "vintage", "viol",
                        "violate", "violation", "violence", "violent", "violet", "violin",
                        "vipers", "virago", "virgin", "virile", "virtual", "virtue",
                        "virtuoso", "virulent", "visage", "viscous", "visible", "vision",
                        "visit", "visitor", "visor", "vista", "visual", "vital",
                        "vitality", "vitals", "vivid", "vividly", "vixen", "vocal",
                        "vocation", "vociferous", "vogue", "voice", "void", "volatile",
                        "volcanic", "volcano", "volley", "volt", "volume", "voluminous",
                        "voluntary", "volunteer", "voluptuous", "vomit", "voodoo",
                        "voracious", "vortex", "vote", "voter", "vouch", "vowel",
                        "voyage", "vulg", "vulgar", "vulnerable", "vulture", "w",
                        "wad", "wade", "wafer", "wag", "wage", "wager", "wagon",
                        "wail", "waist", "wait", "waiter", "waiting", "wake", "wale",
                        "walk", "wall", "wallet", "wallop", "wallow", "walnut",
                        "walrus", "waltz", "wan", "wand", "wander", "wane", "want",
                        "wanton", "war", "ward", "warden", "wardrobe", "ware", "warfare",
                        "warm", "warmth", "warn", "warning", "warp", "warrant", "warren",
                        "wary", "wash", "wasp", "waste", "waster", "watch", "watchful",
                        "water", "waterfall", "watermelon", "wave", "waver", "wax",
                        "way", "wayfarer", "wayward", "we", "weak", "weaken", "weakness",
                        "weal", "wealth", "wealthy", "wean", "weapon", "wear",
                        "weariness", "weary", "weather", "weave", "weaver", "web",
                        "wed", "wedding", "wedge", "wedlock", "wee", "weed", "week",
                        "weekly", "weep", "weigh", "weight", "weighty", "weird",
                        "welcome", "weld", "welfare", "well", "wellnigh", "welt",
                        "wench", "wend", "went", "wept", "were", "west", "western",
                        "wet", "whale", "whaler", "wharf", "what", "whatever",
                        "whatsoever", "wheat", "wheedle", "wheel", "wheeze", "whelp",
                        "when", "whence", "whenever", "whensoever", "where", "whereat",
                        "whereby", "wherefore", "wherein", "whereof", "whereon",
                        "wheresoever", "whereupon", "wherever", "wherewith", "whet",
                        "whether", "whetstone", "whew", "whey", "which", "whichever",
                        "whiff", "while", "whim", "whimper", "whimsical", "whine",
                        "whining", "whip", "whir", "whirl", "whirlpool", "whirlwind",
                        "whisk", "whisker", "whiskey", "whisper", "whistle", "white",
                        "whiten", "whither", "whitish", "whittle", "whiz", "who",
                        "whoa", "whoever", "whole", "wholehearted", "wholly", "whom",
                        "whomever", "whoop", "whooping", "whose", "whosoever", "why",
                        "wicked", "wickedness", "wide", "widely", "widen", "widow",
                        "widower", "width", "wield", "wife", "wig", "wiggle",
                        "wild", "wilder", "wilderness", "wildly", "wile", "will",
                        "willing", "willow", "wily", "win", "wince", "wind",
                        "windmill", "window", "wine", "wing", "wink", "winner",
                        "winning", "winter", "wipe", "wire", "wisdom", "wise",
                        "wish", "wisp", "wistful", "wit", "witch", "witchcraft",
                        "with", "withdraw", "withdrawal", "withe", "withhold",
                        "within", "without", "withstand", "witness", "witticism",
                        "wittingly", "witty", "wives", "woe", "woeful", "wolf",
                        "woman", "womanly", "womb", "won", "wonder", "wonderful",
                        "wondrous", "wont", "woo", "wood", "wooden", "woodland",
                        "woodman", "woody", "wool", "woolen", "word", "wordy",
                        "wore", "work", "worker", "working", "workman", "world",
                        "worldly", "worm", "worn", "worry", "worse", "worship",
                        "worst", "worth", "worthless", "worthy", "would", "wound",
                        "wove", "woven", "wr", "wrack", "wraith", "wrangle", "wrap",
                        "wrapped", "wrapper", "wrapping", "wrath", "wreak", "wreath",
                        "wreathe", "wreck", "wreckage", "wrench", "wrest", "wrestle",
                        "wretch", "wretched", "wriggle", "wring", "wrinkle", "wrist",
                        "writ", "write", "writer", "writhe", "writing", "written",
                        "wrong", "wrongful", "wrongly", "wrote", "wrought", "wrung",
                        "wry", "y", "yam", "yankee", "yard", "yarn", "yawl",
                        "yawn", "ye", "yea", "year", "yearly", "yearn", "yeast",
                        "yell", "yellow", "yelp", "yeoman", "yes", "yest",
                        "yesterday", "yet", "yield", "yoke", "yolk", "yon",
                        "yonder", "you", "young", "younger", "youngest", "your",
                        "yours", "yourself", "yourselves", "youth", "youthful",
                        "yule", "zany", "zeal", "zealot", "zealous", "zebra",
                        "zenith", "zephyr", "zinc", "zodiac", "zone", "zonked",
                        "zoom" // Adicionando mais palavras para ter uma medição mais significativa
                        };


    // Lista de palavras para buscar (algumas presentes, outras não)
    char search_words[][32] = {"the", "these", "their", "thaw", "any", "by", "bye", "them",
                               "algorithm", "structure", "data", "performance", "timing",
                               "zebra", "youth", "quick", "brown", "fox", "jumps",
                               "over", "the", "lazy", "dogs", "trie", "node",
                               "children", "alphabet", "size", "insert", "search",
                               "freememory", "example", "function", "pointer", "malloc",
                               "calloc", "sizeof", "memcpy", "time", "monotonic",
                               "nanoseconds", "seconds", "elapsed", "duration", "code",
                               "programming", "language", "computer", "science", "engineer",
                               "professor", "student", "learning", "knowledge", "expert"};


    struct TrieNode *root = createNode();
    int num_words = sizeof(words) / sizeof(words[0]);
    int num_search_words = sizeof(search_words) / sizeof(search_words[0]);

    struct timespec start_insert, end_insert;
    struct timespec start_search, end_search;
    double elapsed_insert, elapsed_search;

    // --- Medição do tempo de inserção ---
    printf("Iniciando inserção de %d palavras na Trie...\n", num_words);
    clock_gettime(CLOCK_MONOTONIC, &start_insert);

    for (int i = 0; i < num_words; i++) {
        insert(root, words[i]);
        // printf("Inserido: \"%s\"\n", words[i]); // Descomente para ver as palavras sendo inseridas
    }

    clock_gettime(CLOCK_MONOTONIC, &end_insert);
    printf("Inserção concluída.\n");

    // Calcular o tempo decorrido para inserção
    elapsed_insert = (end_insert.tv_sec - start_insert.tv_sec) +
                     (end_insert.tv_nsec - start_insert.tv_nsec) / 1e9;

    printf("Tempo de Inserção: %.9f segundos\n", elapsed_insert);
    printf("\n");

    // --- Medição do tempo de busca ---
    printf("Iniciando busca por %d palavras na Trie...\n", num_search_words);
    clock_gettime(CLOCK_MONOTONIC, &start_search);

    for (int i = 0; i < num_search_words; i++) {
        search(root, search_words[i]);
        // Descomente as linhas abaixo se quiser ver os resultados individuais da busca
        // if (search(root, search_words[i])) {
        //     printf("Busca por \"%s\": ENCONTRADO\n", search_words[i]);
        // } else {
        //     printf("Busca por \"%s\": NAO ENCONTRADO\n", search_words[i]);
        // }
    }

    clock_gettime(CLOCK_MONOTONIC, &end_search);
    printf("Busca concluída.\n");

    // Calcular o tempo decorrido para busca
    elapsed_search = (end_search.tv_sec - start_search.tv_sec) +
                     (end_search.tv_nsec - start_search.tv_nsec) / 1e9;

    printf("Tempo de Busca: %.9f segundos\n", elapsed_search);
    printf("\n");


    // --- Exemplo de prefixo (que não é palavra) ---
    // Não incluímos na medição de tempo principal para manter o foco nas operações em massa
    printf("Buscando prefixo \"ther\" (exemplo isolado):\n");
    if (search(root, "ther")) {
         printf("Busca por \"ther\": ENCONTRADO (mas \"ther\" é um prefixo, não palavra completa neste exemplo)\n");
    } else {
         printf("Busca por \"ther\": NAO ENCONTRADO (correto, pois \"ther\" não foi inserida como palavra completa)\n");
    }
    printf("\n");


    // --- Liberar memória ---
    printf("Liberando memória da Trie...\n");
    freeTrie(root);
    printf("Memória liberada.\n");


    return 0;
}
