
#define TEMPERATURE_UNDER 			0       // 0.0 laipsniai - apatine riba
#define TEMPERATURE_OVER 			1500    // 150.0 laipsniai - virsutine riba
#define TEMPERATURE_TABLE_START 	0       // 0.0 laipsniai - pirma lenteles reiksme
#define TEMPERATURE_TABLE_STEP 		50      // 5.0 laipsniai zingsnis

typedef uint16_t TEMPERATURE_TABLE_ENTRY_T;
typedef uint8_t TEMPERATURE_TABLE_INDEX_T;


/* Таблица суммарного значения АЦП в зависимости от температуры. От большего значения к меньшему
   Для построения таблицы использованы следующие парамертры:
    R1(T1): 10кОм(25°С)
    B25/50: 3380
    Схема включения: A
    Ra: 8.2кОм
    Напряжения U0/Uref: 5В/5В
    Множитель результата АЦП: 64
*/
__flash TEMPERATURE_TABLE_ENTRY_T termo_table[] = {
    50782, 48085, 45204, 42192, 39107, 36009, 32956, 29999,
    27178, 24524, 22058, 19790, 17722, 15850, 14166, 12658,
    11313, 10117,  9054,  8112,  7277,  6538,  5883,  5302,
     4787,  4329,  3923,  3560,  3238,  2950,  2692
};

int16_t calc_temperature(TEMPERATURE_TABLE_ENTRY_T adcsum){
	TEMPERATURE_TABLE_INDEX_T l = 0;
	TEMPERATURE_TABLE_INDEX_T r = ( sizeof(termo_table) / sizeof(termo_table[0]) ) - 1;
    
	TEMPERATURE_TABLE_ENTRY_T thigh = termo_table[r];
	
	if(adcsum <= thigh){
        
#ifdef TEMPERATURE_UNDER
		if(adcsum < thigh) 
			return TEMPERATURE_UNDER;
#endif
		return TEMPERATURE_TABLE_STEP * r + TEMPERATURE_TABLE_START;
	}
	
	TEMPERATURE_TABLE_ENTRY_T tlow = termo_table[0];
	
	if(adcsum >= tlow){
		
#ifdef TEMPERATURE_OVER
		if(adcsum > tlow)
			return TEMPERATURE_OVER;
#endif
		
		return TEMPERATURE_TABLE_START;
	}
	
	while((r - l) > 1) {
		
		TEMPERATURE_TABLE_INDEX_T m = (l + r) >> 1;
		
		TEMPERATURE_TABLE_ENTRY_T mid = termo_table[m];
		
		if(adcsum > mid) r = m;
		else l = m;
	}
	
	TEMPERATURE_TABLE_ENTRY_T vl = termo_table[l];
	
	if (adcsum >= vl) {
		return l * TEMPERATURE_TABLE_STEP + TEMPERATURE_TABLE_START;
	}
	
	TEMPERATURE_TABLE_ENTRY_T vr = termo_table[r];
	
	TEMPERATURE_TABLE_ENTRY_T vd = vl - vr;
	
	int16_t res = TEMPERATURE_TABLE_START + r * TEMPERATURE_TABLE_STEP;
	
	if(vd) {
		res -= ((TEMPERATURE_TABLE_STEP * (int32_t)(adcsum - vr) + (vd >> 1)) / vd);
	}
	
	return res;
}