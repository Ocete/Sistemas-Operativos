### 18. Supongamos que tenemos un proceso ejecutándose en un sistema paginado, con gestión de memoria basada en el algoritmo de sustitución frecuencia de faltas de página. El proceso tiene 5 páginas ( 0, 1, 2, 3, 4 ). Represente el contenido de la memoria real para ese proceso (es decir, indique que páginas tiene cargadas en cada momento) y cuándo se produce una falta de página. Suponga que, inicialmente, está cargada la página 2, el resto de páginas están en memoria secundaria y que no hay restricciones en cuanto al número de marcos de página disponibles. La cadena de referencias a página es: 0 3 1 1 1 3 4 4 2 2 4 0 0 0 0 3 y el parámetro es τ=3.

| Referencias | 0 | 3 | 1 | 1 | 1 | 3 | 4 | 4 | 2 | 2 | 4 | 0 | 0 | 0 | 0 | 3 |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| 0 | `0` | 0 | 0 | 0 | 0 | 0 | - | - | - | - | - | `0` | `0` | `0` | `0` | 0 |
| 1 | - | - | `1` | `1` | `1` | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 | 1 |
| 2 | 2 | 2 | 2 | 2 | 2 | 2 | - | - | `2` | `2` | 2 | 2 | 2 | 2 | 2 | 2 |
| 3 | - | `3` | 3 | 3 | 3 | `3` | 3 | 3 | 3 | 3 | 3 | 3 | 3 | 3 | 3 | `3` |
| 4 | - | - | - | - | - | - | `4` | `4` | 4 | 4 | `4` | 4 | 4 | 4 | 4 | 4 |
| Faltas de ṕagina | * | * | * |  |  |  | * |  | * |  |  | * |  |  |  |  |


Cada fila (0 - 4) de la tabla representa una página en memoria.  
Cuando se una falta de pagina se consulta hace cuanto tiempo de la anterior falta de pagina. Si la diferencia es menor o igual que T (= 3) se añade la pagina a las actuales, si es estrictamente mayor, se quedan en cache unicamente las paginas referenciadas entre el instante actual y la anterior falta de pagina.
