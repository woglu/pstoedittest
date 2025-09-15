
enum  paperUnits { p_mm, p_in };
struct PaperInfo {
	paperUnits preferredunit;
	double mmw;
	double mmh;
	double inw;
	double inh;
	const char * name;
};

const double mm_per_inch = 2.54;
const PaperInfo paperformats[] = {

//{ p_mm, Metric Width,	Metric Height,	Inches Width,	Inches Height,	"Paper Type"},
	{ p_in, (double)8.5* mm_per_inch,	(double)11* mm_per_inch,	8.5,	11,	"A"},
	{ p_in, (double)11* mm_per_inch,	(double)17* mm_per_inch,	11,	17,	"B"},
	{ p_in, (double)17* mm_per_inch,	(double)22* mm_per_inch,	17,	22,	"C"},
	{ p_in, (double)22* mm_per_inch,	(double)34* mm_per_inch,	22,	34,	"D"},
	{ p_in, (double)34* mm_per_inch,	(double)44* mm_per_inch,	34,	44,	"E"},
	{ p_mm, 104.7,	241.3,	4.125,	9.5,	"Com 10"},
	{ p_mm, 190.5,	266.7,	7.25,	10.5,	"Executive"},
	{ p_in, (double)14.875* mm_per_inch,	(double)11* mm_per_inch,	14.875,	11,	"Fanfold"},
	{ p_mm, 215.9,	330.2,	8.5,	13,	"Folio"},
	{ p_in, (double)8.5* mm_per_inch,	(double)12* mm_per_inch,	8.5,	12,	"German standard fanfold"},
	{ p_in, (double)17* mm_per_inch,	(double)11* mm_per_inch,	17,	11,	"Ledger"},
	{ p_mm, 215.9,	355.6,	8.5,	14,	"Legal"},
	{ p_in, (double)9.5* mm_per_inch,	(double)15* mm_per_inch,	9.5,	15,	"Legal extra"},
	{ p_mm, 215.9,	279.4,	8.5,	11,	"Letter"},
	{ p_in, (double)9.5* mm_per_inch,	(double)12* mm_per_inch,	9.5,	12,	"Letter extra"},
	{ p_in, (double)8.5* mm_per_inch,	(double)12.66* mm_per_inch,	8.5,	12.66,	"Letter plus"},
	{ p_mm, 98.4,	190.5,	3.875,	7.5,	"Monarch"},
	{ p_mm, 215,	275,	(double)215/ mm_per_inch,	(double)275/ mm_per_inch,	"Quarto"},
	{ p_in, (double)5.5* mm_per_inch,	(double)8.5* mm_per_inch,	5.5,	8.5,	"Statement"},
	{ p_in, (double)11* mm_per_inch,	(double)17* mm_per_inch,	11,	17,	"Tabloid"},
	{ p_in, (double)11.66* mm_per_inch,	(double)18* mm_per_inch,	11.66,	18,	"Tabloid extra"},
	{ p_mm, 841,	1189,	(double)841/ mm_per_inch,	(double)1189/ mm_per_inch,	"A0"},
	{ p_mm, 594,	841,	(double)594/ mm_per_inch,	(double)841/ mm_per_inch,	"A1"},
	{ p_mm, 420,	594,	(double)420/ mm_per_inch,	(double)594/ mm_per_inch,	"A2"},
	{ p_mm, 297,	420,	(double)297/ mm_per_inch,	(double)420/ mm_per_inch,	"A3"},
	{ p_mm, 322,	445,	(double)322/ mm_per_inch,	(double)445/ mm_per_inch,	"A3 extra"},
	{ p_mm, 210,	297,	(double)210/ mm_per_inch,	(double)297/ mm_per_inch,	"A4"},
	{ p_mm, 236,	322,	(double)236/ mm_per_inch,	(double)322/ mm_per_inch,	"A4 extra"},
	{ p_mm, 210,	330,	(double)210/ mm_per_inch,	(double)330/ mm_per_inch,	"A4 plus"},
	{ p_mm, 210,	297,	(double)210/ mm_per_inch,	(double)297/ mm_per_inch,	"A4 small"},
	{ p_mm, 148,	210,	(double)148/ mm_per_inch,	(double)210/ mm_per_inch,	"A5"},
	{ p_mm, 174,	235,	(double)174/ mm_per_inch,	(double)235/ mm_per_inch,	"A5 extra"},
	{ p_mm, 105,	148,	(double)105/ mm_per_inch,	(double)148/ mm_per_inch,	"A6"},
	{ p_mm, 74,	105,	(double)74/ mm_per_inch,	(double)105/ mm_per_inch,	"A7"},
	{ p_mm, 52,	74,	(double)52/ mm_per_inch,	(double)74/ mm_per_inch,	"A8"},
	{ p_mm, 37,	52,	(double)37/ mm_per_inch,	(double)52/ mm_per_inch,	"A9"},
	{ p_mm, 26,	37,	(double)26/ mm_per_inch,	(double)37/ mm_per_inch,	"A10"},
	{ p_mm, 18,	26,	(double)18/ mm_per_inch,	(double)26/ mm_per_inch,	"A11"},
	{ p_mm, 13,	18,	(double)13/ mm_per_inch,	(double)18/ mm_per_inch,	"A12"},
	{ p_mm, 99,	210,	(double)99/ mm_per_inch,	(double)210/ mm_per_inch,	"1/3 A4"},
	{ p_mm, 74,	210,	(double)74/ mm_per_inch,	(double)210/ mm_per_inch,	"1/4 A4"},
	{ p_mm, 13,	74,	(double)13/ mm_per_inch,	(double)74/ mm_per_inch,	"1/8 A4"},
	{ p_mm, 1000,	1414,	(double)1000/ mm_per_inch,	(double)1414/ mm_per_inch,	"B0"},
	{ p_mm, 707,	1000,	(double)707/ mm_per_inch,	(double)1000/ mm_per_inch,	"B1"},
	{ p_mm, 500,	707,	(double)500/ mm_per_inch,	(double)707/ mm_per_inch,	"B2"},
	{ p_mm, 353,	500,	(double)353/ mm_per_inch,	(double)500/ mm_per_inch,	"B3"},
	{ p_mm, 250,	353,	(double)250/ mm_per_inch,	(double)353/ mm_per_inch,	"B4"},
	{ p_mm, 176,	250,	(double)176/ mm_per_inch,	(double)250/ mm_per_inch,	"B5"},
	{ p_mm, 201,	276,	(double)201/ mm_per_inch,	(double)276/ mm_per_inch,	"B5 extra"},
	{ p_mm, 125,	176,	(double)125/ mm_per_inch,	(double)176/ mm_per_inch,	"B6"},
	{ p_mm, 88,	125,	(double)88/ mm_per_inch,	(double)125/ mm_per_inch,	"B7"},
	{ p_mm, 62,	88,	(double)62/ mm_per_inch,	(double)88/ mm_per_inch,	"B8"},
	{ p_mm, 44,	62,	(double)44/ mm_per_inch,	(double)62/ mm_per_inch,	"B9"},
	{ p_mm, 31,	44,	(double)31/ mm_per_inch,	(double)44/ mm_per_inch,	"B10"},
	{ p_mm, 162,	229,	(double)162/ mm_per_inch,	(double)229/ mm_per_inch,	"C5"},
	{ p_mm, 200,	210,	(double)200/ mm_per_inch,	(double)210/ mm_per_inch,	"1/6 Deutsche Industrie Norm"},
	{ p_mm, 74,	105,	(double)74/ mm_per_inch,	(double)105/ mm_per_inch,	"A7 envelope"},
	{ p_mm, 74,	148,	(double)74/ mm_per_inch,	(double)148/ mm_per_inch,	"A7/A6 envelope"},
	{ p_mm, 250,	353,	(double)250/ mm_per_inch,	(double)353/ mm_per_inch,	"B4 envelope"},
	{ p_mm, 176,	250,	(double)176/ mm_per_inch,	(double)250/ mm_per_inch,	"B5 envelope"},
	{ p_mm, 125,	176,	(double)125/ mm_per_inch,	(double)176/ mm_per_inch,	"B6 envelope"},
	{ p_mm, 125,	229,	(double)125/ mm_per_inch,	(double)229/ mm_per_inch,	"B6/C4 envelope"},
	{ p_mm, 136,	353,	(double)136/ mm_per_inch,	(double)353/ mm_per_inch,	"1/2 B4 envelope"},
	{ p_mm, 484,	324,	(double)484/ mm_per_inch,	(double)324/ mm_per_inch,	"C3 envelope"},
	{ p_mm, 229,	324,	(double)229/ mm_per_inch,	(double)324/ mm_per_inch,	"C4 envelope"},
	{ p_mm, 162,	229,	(double)162/ mm_per_inch,	(double)229/ mm_per_inch,	"C5 envelope"},
	{ p_mm, 114,	162,	(double)114/ mm_per_inch,	(double)162/ mm_per_inch,	"C6 envelope"},
	{ p_mm, 114,	229,	(double)114/ mm_per_inch,	(double)229/ mm_per_inch,	"C65 envelope"},
	{ p_mm, 81,	114,	(double)81/ mm_per_inch,	(double)114/ mm_per_inch,	"C7 envelope"},
	{ p_mm, 81,	162,	(double)81/ mm_per_inch,	(double)162/ mm_per_inch,	"C7/C6 envelope"},
	{ p_mm, 125,	324,	(double)125/ mm_per_inch,	(double)324/ mm_per_inch,	"1/2 C4 envelope"},
	{ p_mm, 110,	220,	(double)110/ mm_per_inch,	(double)220/ mm_per_inch,	"DL envelope"},
	{ p_mm, 110,	220,	(double)110/ mm_per_inch,	(double)220/ mm_per_inch,	"Deutsche Industrie Norm Lang envelope"},
	{ p_mm, 310,	220,	(double)310/ mm_per_inch,	(double)220/ mm_per_inch,	"E4 envelope"},
	{ p_mm, 220,	115,	(double)220/ mm_per_inch,	(double)115/ mm_per_inch,	"E5 envelope"},
	{ p_mm, 110,	156,	(double)110/ mm_per_inch,	(double)156/ mm_per_inch,	"E6 envelope"},
	{ p_mm, 110,	220,	(double)110/ mm_per_inch,	(double)220/ mm_per_inch,	"E65 envelope"},
	{ p_mm, 155,	400,	(double)155/ mm_per_inch,	(double)400/ mm_per_inch,	"1/2 E4 envelope"},
	{ p_mm, 104,	160,	(double)104/ mm_per_inch,	(double)160/ mm_per_inch,	"Hungarian Air Mail envelope"},
	{ p_mm, 220,	220,	(double)220/ mm_per_inch,	(double)220/ mm_per_inch,	"Invitation envelope"},
	{ p_mm, 155,	220,	(double)155/ mm_per_inch,	(double)220/ mm_per_inch,	"M5 envelope"},
	{ p_mm, 110,	155,	(double)110/ mm_per_inch,	(double)155/ mm_per_inch,	"M6 envelope"},
	{ p_mm, 110,	220,	(double)110/ mm_per_inch,	(double)220/ mm_per_inch,	"M65 envelope"},
	{ p_in, (double)3.875* mm_per_inch,	(double)7.5* mm_per_inch,	3.875,	7.5,	"Monarch envelope"},
	{ p_mm, 187,	122,	(double)187/ mm_per_inch,	(double)122/ mm_per_inch,	"Russian Greetings 1 envelope"},
	{ p_in, (double)3.625* mm_per_inch,	(double)6.5* mm_per_inch,	3.625,	6.5,	"Size 6 3/4 envelope"},
	{ p_in, (double)3.575* mm_per_inch,	(double)8.875* mm_per_inch,	3.575,	8.875,	"Size 9 envelope"},
	{ p_in, (double)4.125* mm_per_inch,	(double)9.5* mm_per_inch,	4.125,	9.5,	"Size 10 envelope"},
	{ p_in, (double)4.5* mm_per_inch,	(double)10.375* mm_per_inch,	4.5,	10.375,	"Size 11 envelope"},
	{ p_in, (double)4.75* mm_per_inch,	(double)11* mm_per_inch,	4.75,	11,	"Size 12 envelope"},
	{ p_in, (double)5* mm_per_inch,	(double)11.5* mm_per_inch,	5,	11.5,	"Size 14 envelope"},
{ p_in, 0, 0, 0, 0, nullptr }

};

static const PaperInfo* getPaperInfo(const char * pname) {
	const PaperInfo * pi = &paperformats[0];
	while (pi && pi->name) {
		if (STRICMP(pi->name,pname) == 0) return pi;
		pi++;
	}
	cerr << "could not find paper info for page size " << pname << endl;
	return nullptr;
}


