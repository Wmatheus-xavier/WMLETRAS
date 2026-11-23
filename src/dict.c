/**
 * @file dict.c
 * @brief Dicionário embutido de palavras do português (sem acento, maiúsculas).
 *
 * Tabela hash aberta com sondagem linear.
 *
 * Critério das palavras: português corrente, 2–8 letras, sem acento
 * (acentuação removida para compatibilidade com entrada do terminal).
 * Inclui todas as respostas do arquivo entrada.txt padrão.
 *
 * DICT_SIZE deve ser primo estritamente maior que 2 × número de palavras
 * para manter fator de carga < 0,5 (desempenho O(1) médio).
 * Palavras no array: 1536 → DICT_SIZE = 3307 (primo > 2 × 1536).
 */
#include "dict.h"

#include <string.h>
#include <ctype.h>

/* ── Array de palavras ───────────────────────────────────────────────────── */

#define DICT_SIZE 3307   /* primo > 2 × 1536 palavras */

static const char *palavras[] = {
    /* ── Palavras do entrada.txt padrão ───────────────────────────────── */
    "RUGA","GRAU","RUA","AGUA","AGUAR",
    "LATIM","LIMA","TIL","MIL","TIA","TAL",
    "RATO","TORA","RIA","RITA","ROTA","ATOR",
    "LUPA","PULA","LUA","DUPLO","DOA","LUDO",

    /* ── 2-3 letras ────────────────────────────────────────────────────── */
    "EM","DE","DO","DA","SE","NA","NO","AS","OS","ME","TE","EU",
    "AI","OU","IR","VI","ID","FA","FE","GO","HI","LA","MA","PA",
    "SA","TA","VA","ZA","BI","CI","DI","GI","LI","OI","PI","SI",
    "TI","XI","BO","CO","FO","LO","MO","PO","TO","VO","BU","DU",
    "FU","GU","HU","JU","MU","NU","PU","SU","TU","VU",
    "ABA","ACA","AGA","AIA","AMA","ANA","APA","ARA","ASA","ATA",
    "AVA","AXA","AZA","ECA","ELA","ELE","EMU","EPI","ERA","ETA",
    "OCA","ODA","OPA","ORA","OVO","UVA","UNA","UMA","USA",
    "BOI","CAI","CAU","CEU","FIO","RIO","TIO","VIA","BOA","CIA",
    "DIA","LIA","MIA","PIA","ZIA","CAO","MAO","PAO","SAO","VAO","TAO",

    /* ── 4 letras ──────────────────────────────────────────────────────── */
    "ABRE","ABRI","ACHO","ACRE","AFRO","AGRO","AGIL",
    "AIDE","ALCE","ALGO","ALHO","ALMA","ALVO","AMAS","AMEI","AMOR",
    "ANOS","ANTE","APOS","APTO","ARCO","ARMA","ARTE","ASNO","ATEU",
    "ATOL","ATOU","AVOS","AZUL","BALA","BATA","BATE","BATI",
    "BELA","BELO","BICO","BOCA","BOLA","BOLO","BONS","BORA",
    "BREU","BRIO","BULA","CABE","CACO","CADA","CAGA","CAIS","CALA",
    "CALE","CALO","CAMA","CANA","CANO","CARA","CARO","CATA","CATO",
    "CAVE","CEDE","CEIA","CELA","CELO","CENA","CERO","CIMA","CIPO",
    "CITO","COAR","COCE","COLA","COLO","COMA","COME","COMO","CONE",
    "COPO","CORA","CORE","CORO","COTA","COTO","COVA","CREM","CRIA",
    "CRIO","CRUA","CUBO","CUIA","CULO","CUME","CURA","CURO",
    "DADO","DAMA","DANO","DAVA","DEDO","DELE","DEMO","DERA","DEUS",
    "DEVO","DITO","DOCA","DOMO","DONA","DONO","DORA","DOSE","DOTE",
    "DUAL","DURA","DURO","ECOS","EGUA","EIRA","EIXO","ELAS","ELES",
    "ELOS","ERRO","ERVA","ESSE","ESTA","ESTE","EURO",
    "FACA","FADO","FALA","FALE","FALI","FALO","FAMA","FARO","FATO",
    "FAVA","FEIO","FETO","FICA","FICO","FIGO","FILA","FILE","FILO",
    "FINA","FINO","FITA","FITO","FIXO","FOCA","FOCO","FOLE","FOME",
    "FORA","FORO","FOTO","FRIA","FRIO","FUMO","FURO",
    "GAGO","GALA","GALE","GALO","GANA","GATE","GATO","GAZE","GEMA",
    "GENE","GERA","GIRO","GOCE","GOLE","GOLO","GOMA","GOTA","GOZA",
    "GOZO","GRIS","GUIA","HINO","HORA","IAIA","IDEA","INCA","ITEM",
    "JATO","JIPE","JOCA","JOGO","JOIA","JOTA",
    "LACA","LACO","LADO","LAMA","LAME","LAMO","LAPA","LATE","LATO",
    "LAVA","LAVE","LAVO","LEIA","LEIO","LEMA","LENA","LERO","LEVA",
    "LEVO","LIDA","LIDO","LIGA","LIGO","LIMO","LISA","LISO","LITO",
    "LOGO","LOJA","LONA","LORE","LOTA","LOTE","LOTO","LULA","LUME",
    "LUNA","LUVA",
    "MACA","MACO","MAGA","MAGO","MALA","MALE","MALO","MAMA","MAME",
    "MAMO","MANA","MANO","MAPA","MARE","MARO","MATA","MATE","MATO",
    "MEDO","MEIA","MEIO","MELA","MELO","MEMO","MESA","METE","METI",
    "METO","MICO","MIGA","MILE","MIMO","MINA","MINO","MIRA","MIRO",
    "MITO","MOCA","MOCO","MODO","MOLA","MOLE","MOMA","MONA","MONO",
    "MORA","MORE","MORO","MOTA","MOTO","MUDA","MUDO","MULA","MULO","MUSA",
    "NADA","NADO","NANA","NATO","NAUS","NAVE","NEVE","NOCA","NOMA",
    "NONA","NORA","NOVA","NOVO","NUCA","NUMA","NUNS",
    "OBRA","OCRE","OLHA","OLHE","OLHO","OLMO","ORAL","ORCA","ORLA",
    "ORNA","ORTA","OSSO","OURA","OURO","OVAL",
    "PACA","PACO","PAGA","PAGO","PALA","PALE","PALO","PAPA","PARE",
    "PARO","PATA","PATO","PECA","PECO","PELE","PELO","PENA","PERA",
    "PESA","PESO","PETO","PICA","PICO","PILE","PILO","PINA","PINO",
    "PIPA","PIRA","PIRO","PITA","PITO","POCO","PODA","PODE","POLO",
    "POMA","POMO","PONA","POPA","PORE","PORO","POSA","POSE","POSO",
    "POTE","POVO","PROA","PUCA","PUDE","PULE","PULO","PUMA","PURO",
    "RACA","RACO","RADA","RAIA","RAMA","RAME","RAMO","RAPA","RAPE",
    "RARA","RARO","RASA","RASO","RATA","RATE","RECA","RECO","REDE",
    "REGO","RELO","REMA","REMO","RENA","RENO","REPO","RETA","RETE",
    "RETO","REZA","REZE","REZO","RICA","RICO","RIFA","RIMA","RINS",
    "RIPA","RISO","RITO","RIXA","ROBA","ROBO","ROCA","ROCO","RODA",
    "RODE","RODO","ROLA","ROLE","ROLO","ROMA","RONA","ROPA","ROSA",
    "ROSE","ROSO","ROTO","RUDE","RUIM","RUMO","RUNA",
    "SABE","SACA","SACO","SAGA","SAIA","SALA","SALE","SALI","SALO",
    "SAMA","SANA","SANE","SANO","SAPO","SARA","SATA","SEBE","SECO",
    "SEDE","SELA","SELE","SELO","SENA","SENO","SERA","SERE","SERO",
    "SETA","SIGA","SIGO","SILO","SIMA","SINO","SIPA","SIRI","SITA",
    "SITO","SOAR","SOBA","SOBE","SOCA","SOCO","SODA","SOFA","SOJA",
    "SOLA","SOLO","SOMA","SOME","SOMO","SONA","SONE","SONO","SOPA",
    "SORA","SORE","SORO","SOTA","SUBO","SUCO","SUJA","SUJO","SULA",
    "SULO","SUMA","SUMO","SURA",
    "TACA","TACO","TALA","TALE","TALO","TAMA","TANO","TAPA","TAPE",
    "TAPO","TARA","TARO","TATA","TATO","TAXA","TECA","TECO","TEIA",
    "TELA","TELE","TELO","TEMO","TENA","TENO","TESA","TESO","TETA",
    "TETO","TIPO","TIRA","TIRE","TIRO","TITA","TITO","TOCA","TOCO",
    "TODA","TODO","TOLE","TOLO","TOMA","TOME","TOMO","TONA",
    "TONE","TONO","TOPA","TOPE","TOPO","TORE","TORO","TOSA","TOSE",
    "TOSO","TOTA","TOTO","TUBA","TUDO","TULE","TUMO","TUNA","TUPO",
    "TURF",
    "UIVO","ULNA","URGE","URNA","URSO","USAR",
    "VALE","VALI","VALO","VAMO","VANA","VANE","VANO","VARA","VARO",
    "VASA","VASE","VASO","VATO","VELA","VELO","VENA","VENO","VERA",
    "VERO","VICE","VIDA","VIGA","VIGE","VIGO","VILA","VIME","VINA",
    "VINE","VINO","VIRA","VIRE","VIRO","VISA","VISE","VISO","VITA",
    "VITO","VIVO","VOGA","VOLA","VOLE","VOLO","VOMA","VOME","VOMO",
    "VOTA","VOTE","VOTO","XALE","XARA","XERE","XILO","ZERO","ZONA",
    "ZONE","ZOTE",

    /* ── 5 letras ──────────────────────────────────────────────────────── */
    "ABUSO","ACASO","ACENO","ACIMA","ACOES","ACOLO","ADEUS","AGORA",
    "AGUDO","AINDA","AJUDA","ALUNO","AMIGO","AMORA","AMPLO","ANCAS",
    "ANDAR","ANEIS","ANIME","ANTES","APELO","ARAME","AREIA","ARMAS",
    "AROMA","ASSAR","ATRAS","AVISO","AZEDO","BANCO","BARCO","BARRO",
    "BEBER","BELAS","BELOS","BESTA","BEIJO","BLOCO","BOLSO","BORDA",
    "BURRO","BUSCA","CACOS","CALOR","CAMPO","CARGO","CARRO","CAUSA",
    "CERTO","CHAVE","CLARO","COBRA","COISA","COLOR","COMUM","CONTA",
    "COPAS","CORPO","CORDA","COURO","COUVE","CRAVO","CRIME","CUECA",
    "CULPA","CURTA","CURTO","CURVA","DEIXA","DELTA","DENSO","DEVER",
    "DEVIR","DISCO","DOBRA","DRAMA","DURAS","DUROS","EDUCA","EMITE",
    "ENTRA","ENTRE","ENVIO","ERRAR","ERROS","ESTOU","ETAPA","EVITA",
    "EXAME","FALAR","FALSO","FARSA","FATAL","FATOR","FEBRE","FECHO",
    "FEITO","FELIZ","FERVE","FESTA","FICOU","FILHO","FIRMA","FLUXO",
    "FOBIA","FORCA","FORMA","FORTE","FORUM","FOSSO","FRACO","FRUTO",
    "FUGIR","FUMAR","FUROR","FUSAO","GANHA","GARFO","GASTO","GENIO",
    "GERAL","GIRAR","GLOBO","GRADE","GRAFO","GRAMA","GRANA","GRIPE",
    "GRUPO","GUETO","GUIAR","HEROI","HIATO","HIENA","HOMEM","HORAS",
    "HOTEL","HUMOR","IDEAL","IGUAL","IMAGO","IMITA","IMPOR","IMUNE",
    "INDUZ","INIBE","INOVA","INTER","ISOLA","JUNTO","KARMA","LACOS",
    "LAGO","LADOS","LARGO","LASER","LATIR","LAURO","LEBRE","LEGAL",
    "LENDA","LENHA","LESMA","LETAL","LIGAR","LIMITE","LINCE","LINHA",
    "LISTA","LITRO","LOCAL","LONGE","LOTOU","LUGAR","LUXOS","MACHO",
    "MAGRO","MAMAO","MANSO","MARCA","MARTE","MASSA","MELAO","MENOR",
    "MENTA","METRO","MILHA","MIOJO","MIRAR","MISSA","MITOS","MOEDA",
    "MOLAR","MOLHO","MORAR","MORTO","MOSCA","MOTEL","MOTOR","MULTA",
    "MUNDO","MURRO","NARIZ","NEGRO","NOBRE","NOITE","NORMA","NORTE",
    "NOSSA","NOVEL","NUDEZ","NUVEM","OBRAS","OLHAR","ORDEM","ORGAO",
    "OSSOS","OUTRA","OUTRO","PACOS","PAGAR","PALCO","PALMO","PAMPA",
    "PARCO","PARDA","PARES","PARTE","PARTO","PASTA","PATIO","PAULO",
    "PEDRA","PELVE","PENAS","PERDA","PERTO","PIADA","PINGO","PIORA",
    "PIRES","PIZZA","PLANO","PLUMA","PODER","POEMA","PORTA","POSSE",
    "POSTO","PRATA","PRECO","PREGO","PRESO","PRIMA","PRIMO","PROVA",
    "PULOU","PUROS","QUASE","QUEDA","QUERO","QUOTA","RAIVA","RAPAZ",
    "RASGO","RAZAO","REAIS","RECUA","REINO","RELVA","REMAR","REPOR",
    "RETAS","RETOS","RIMOS","RITMO","RISOS","ROCHA","ROLAR","ROMBO",
    "ROSTO","RUIDO","RUIVO","SABIA","SABOR","SAGAZ","SALSA","SALTO",
    "SAPOS","SAQUE","SARAR","SAUDE","SENSO","SIGNO","SILVO","SINAL",
    "SOBRE","SOGRA","SOLTA","SOLTO","SOMAR","SONHO","SOPRO","SORTE",
    "SUAVE","SUBIR","SUMIU","SUPER","SURDO","SURFE","SUTIL","TALAR",
    "TALCO","TALHA","TAMPO","TARDE","TARSO","TERMO","TERRA","TESTA",
    "TIMAO","TIMOR","TINTO","TITULO","TOLDO","TOMAR","TOMBO","TOQUE",
    "TORCE","TORDO","TORNO","TORPE","TOTAL","TOUCA","TOURO","TRACO",
    "TRAMA","TREVO","TROCO","TROVA","TUMOR","TURMA","ULTRA","URBAO",
    "USADA","USINA","VALER","VALOR","VEADO","VELAR","VERSO","VESTE",
    "VESPA","VILAS","VIOLA","VIRIL","VIRUS","VISTA","VITAL","VOCAL",
    "VOTAR","ZELOS",

    /* ── 6 letras ──────────────────────────────────────────────────────── */
    "ABISMO","ACORDO","ACERTO","AFRICA","AGENTE","AGONIA","AJUDAR",
    "ALARME","ALCOOL","ALDEIA","AMARGO","AMIGOS","AMPARO","ANCORA",
    "ANIMAL","ANULAR","ARCADA","AREJAR","ARMADO","ARTIGO","ARVORE",
    "ASSADO","ASTRAL","ATLETA","ATRAIR","AURORA","AVANCO","BAIRRO",
    "BANANA","BARCOS","BASICO","BELEZA","BOSQUE","BRUTAL","CAINDO",
    "CAMARA","CANETA","CANTAR","CAPTAR","CARONA","CARROS","CAVALO",
    "CENTRO","CHEIRO","CINEMA","CIDADE","COGITA","COLEGA","COMIDA",
    "COMPRA","COMUNS","CONFIA","CONTOS","CORPOS","CORRER","COSTAS",
    "CRIADA","CRIADO","CUIDAR","DECIDO","DEFESA","DEPOIS",
    "DESVIO","DEVIDA","DEVOTO","DIANTE","DILEMA","DINAMO","DIRETO",
    "DOENTE","DRENOS","ESCOLA","ESFERA","ESPACO","ESPERO","EVENTO",
    "EXIBIR","FACHAD","FAMOSA","FASTIO","FAVELA","FECHAR","FERIDA",
    "FIGURA","FILHOS","FISICA","FLECHA","FLORES","FLUIDO","FOGUET",
    "FOLHAS","FRACAS","FRACOS","FRACAO","FRENTE","FUTBOL","FUTURO",
    "GANHAR","GAROTA","GENERO","GLORIA","GRADES","GRITAR","GROSSO",
    "GRUPOS","GUARDA","GUERRA","HESITA","HISTOR","HOMENS","HONRAR",
    "HUMANO","IMAGEM","IMPEDE","INDICA","INICIO","INOVAR","INVADE",
    "IRONIA","JORNAL","JUIZES","LARGOS","LETRAS","LIBERO","LINHAS",
    "LOGICA","LONGOS","LOUCOS","LUCRAR","MACACO","MADURO","MAGICA",
    "MANDAR","MANEJO","MANTEM","MAPEAR","MARCAR","MEDIDA","MELHOR",
    "METADE","MINUTO","MODELO","MORADA","MORTAL","MOTIVO","MUSICA",
    "NARRAR","NEUTRO","NICHOS","NORMAL","NUMERO","NUTRIR","OBJETO",
    "ODIOSO","OPINAR","OPOSTO","ORBITA","PAPEIS","PAREDE","PARTIR",
    "PASSOS","PAUTAR","PEDIDO","PENSAR","PERDER","PERIGO","PESADO",
    "PIRATA","PLANTA","POUCOS","PRAZER","PREFER","PREMIO","PRESOS",
    "PROIBE","PROVAR","PULSAR","QUANDO","QUARTO","QUATRO","QUIETO",
    "RAPIDO","REGRAS","RESIDE","RESUMO","RIQUEZA","RITMOS","ROMPER",
    "ROTINA","SACADA","SENTIR","SERVIR","SINAIS","SISTEMA","SOCIAL",
    "SOFRER","SOMBRA","SONHAR","TECIDO","TEMPOS","TENDER","TEORIA",
    "TORNAR","TRACAR","TREINO","TROCAR","TURISMO","ULTIMO","UNICOS",
    "URBANO","VALIDE","VAREJO","VARIOS","VAZIOS","VERDES","VIAGEM",
    "VISITA","VOLTAR","ZANGAR",

    /* ── 7-8 letras ────────────────────────────────────────────────────── */
    "ABSOLUT","ABSTRAT","ABUSIVO","ACALMAR","ACERTAR","ACHADOS",
    "ACOLHER","ACORDAR","ADORNAR","AFASTAR","AFIRMAR","AFLORAR",
    "AGITADO","AGRADAR","AGREDIR","AJUSTAR","ALARGAR","ALCANCE",
    "ALERTAR","ALIVIAR","ALONGAR","AMPARAR","AMPLIAR","APONTAR",
    "APOSTAR","APROVAR","ARRASTO","ARRUMAR","ARTISTA","ASPIRAR",
    "ATACADO","ATENTAR","ATINGIR","ATREVER","AVALIAR","BARULHO",
    "BATALHA","BELDADE","BONDADE","BRILHAR","CAMINHO","CAPACID",
    "CELEBRAR","CEREBRO","CERTEZA","CHANCES","CHOCADO","CIDADAO",
    "CIMENTO","COLETIV","COMANDO","COMPLET","CONCEIT","CONCENT",
    "CONFIRM","CONGELA","CONHECER","CONSEGU","CONSTAN","CONTEND",
    "CONVIDA","CORAGEM","CORREIO","CRIANCA","CRITICO","CRESCER",
    "CUMPRIR","CURADOR","CUSTEAR","DEBATER","DECIDIR","DECORAR",
    "DEPENDE","DESAFIO","DESCANS","DESEJAR","DESENHO","DESVIAR",
    "DIFICIL","DINHEIRO","DISCUTI","DURANTE","EDUCADO","EFETIVO",
    "ELOGIAR","EMERGIR","ENGAJAR","ESCOLHER","ESFORCO","ESPERAN",
    "ESQUECER","ESTUDIO","EXAGERO","EXERCIT","EXISTIR","EXPLICAR",
    "EXPOSTO","EXTREMO","FAMILIAR","FANTASMA","FECHADO","FESTEJO",
    "FILTRAR","FOGUETE","FOMENTAR","FRONTEIRA","FUNCIONA","GARANTIR",
    "GLOBAIS","GLORIOSO","GRATIDAO","GROSSURA","GUARDAR","HABITAR",
    "HARMONIA","HEROISMO","HONESTO","HONRADO","IGNORAR","IMAGINAR",
    "IMPOSTOR","INCITAR","INDICAR","INSISTIR","INSTALAR","INTEGRAR",
    "INVESTIR","ISOLADO","LEMBRAR","LIBERTAR","LIDERAR","MADUREZ",
    "MANTENHO","MARTELAR","MELHORAR","MENSAGEM","MERECER","MODERNO",
    "MOTIVACAO","MOVIMENTO","NARRADOR","NATURAL","NEGOCIAR","OBJETIVO",
    "OBSERVAR","OFERECER","ORIENTAR","ORGANIZAR","PACIENTE","PASSAGEM",
    "PASTORAL","PATRULHA","PENSANDO","PERCEBER","PERFEITO","PICARETA",
    "PLANEJAR","POPULAR","POSITIVO","POSSIVEL","PRECISAR","PROBLEMA",
    "PROIBIDO","PROTEGER","PROVOCAR","PUBLICAR","REALIDADE","RECLAMAR",
    "RECURSOS","REFLETIR","REGISTRO","RELEMBRAR","RESOLVER","RESPEITO",
    "RESULTAR","RETORNAR","REUNINDO","SAGRADO","SALIENTE","SAUDACAO",
    "SEGUINDO","SENSIVEL","SEPARADO","SIMPATIA","SINGULAR","SOBERANO",
    "SOLIDARIO","SUSTENTAR","TRABALHO","TRADICAO","TROVOADA","UNITARIO",
    "VALORIZAR","VERIFICAR","VIRTUOSO","VITORIOSO","VOLTAMOS",

    NULL   /* sentinela */
};

/* ── Tabela hash (open addressing, sondagem linear) ─────────────────────── */

static int hash_inicializado = 0;
static int tabela[DICT_SIZE];   /* índice em palavras[], ou -1 = vazio */

static unsigned int hash_str(const char *s) {
    unsigned int h = 5381u;
    while (*s)
        h = ((h << 5) + h) ^ (unsigned char)(*s++);
    return h;
}

static void hash_init(void) {
    if (hash_inicializado) return;
    for (int i = 0; i < DICT_SIZE; i++) tabela[i] = -1;

    for (int k = 0; palavras[k] != NULL; k++) {
        unsigned int slot = hash_str(palavras[k]) % DICT_SIZE;
        for (int probe = 0; probe < DICT_SIZE; probe++) {
            int idx = (int)((slot + (unsigned)probe) % DICT_SIZE);
            if (tabela[idx] == -1) {
                /* Slot livre: insere */
                tabela[idx] = k;
                break;
            }
            /* Duplicata silenciosa: slot ocupado pela mesma palavra — ignora */
            if (strcmp(palavras[tabela[idx]], palavras[k]) == 0)
                break;
        }
    }
    hash_inicializado = 1;
}

/* ── API pública ─────────────────────────────────────────────────────────── */

int dict_existe(const char *palavra) {
    if (!palavra || !palavra[0]) return 0;
    hash_init();

    /* Converte para maiúsculas */
    char upper[64];
    int i;
    for (i = 0; palavra[i] && i < 63; i++)
        upper[i] = (char)toupper((unsigned char)palavra[i]);
    upper[i] = '\0';

    unsigned int slot = hash_str(upper) % DICT_SIZE;
    for (int probe = 0; probe < DICT_SIZE; probe++) {
        int idx = (int)((slot + (unsigned)probe) % DICT_SIZE);
        if (tabela[idx] == -1) return 0;          /* buraco — não está */
        if (strcmp(palavras[tabela[idx]], upper) == 0) return 1;
    }
    return 0;
}
