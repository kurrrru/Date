# エチオピア暦カレンダー実装仕様

## エチオピア暦の特徴
- 13 ヶ月制 (Meskerem〜Pagume)。最初の 12 ヶ月は 30 日固定、13 ヶ月目 (Pagume) は平年 5 日・閏年 6 日です。
- 4 年ごとに閏日を挿入しますが、エチオピア暦では `year % 4 == 3` が閏年に相当します (グレゴリオ暦の 7〜8 年遅れ)。
- 実装は **先発エチオピア暦** で、ユリウス暦 AD 8/8/29 を epoch として全範囲に適用しています。
- `Era` は `BC`/`AD` の 2 種を用意していますが、実装上は AD 8 年 8 月 29 日 (ユリウス暦) 以降のシリアル値のみを扱います。

## 実装構成
### 日付計算ロジック
- `EthiopianCalendar::to_serial_date` はユリウス暦 `AD 8-08-29` を基準に、
	- `(year-1)*365 + floor((year-1)/4)` で年累積日数、
	- `30*(month-1)` と `(day-1)` で月日を加算しシリアル日を得ます。
	- 13 ヶ月目の上限は `last_day_of_month` で 5 or 6 日に制限しています。
- `from_serial_date` は逆変換を行いますが、`serial_date < epoch` の場合は `std::out_of_range` を投げます (BC 変換は未対応)。
- 曜日計算は他暦と同じくシリアル値の剰余計算を利用します。

### フォーマット/パース機構
- `to_serial_date(const std::string&, const char*, bool)` と `parse_*` 群は再帰 descent で入力文字列を解析し、`%E/%Y/%M/%D` といった指定子をサポートします。
- 大文字指定子 (`%Y`, `%M`, `%D`) は可変長の算用数字を、 小文字指定子は固定フォーマットを採用しています。
	- `%m` は月名 (`Meskerem`, `Pagume` など) を扱い、`%M` は 1〜13 の数値。
	- `%d` は 2 桁固定の数字を期待し、`%D` は桁数可変。
- `strict=true` (既定) では複数解があるパターンを例外として扱い、`strict=false` では先にマッチした解釈で確定します。リテラルや空白の揺れは許容されません。
- `from_serial_date(..., const char* format)` は同じ指定子集合で文字列を生成し、曜日指定子 `%W/%w` をサポートします (詳細は後述の表を参照)。

### 主なクラス
- `EthiopianCalendar` は `ICalendarSystem` を実装する先発エチオピア暦クラスで、`Date` クラスの `CalendarSystem::ETHIOPIAN` から利用されます。
- 変換には内部で `JulianCalendar` を使用しており、エチオピア暦とユリウス暦間の epoch を固定参照します。

## 実装しているもの
- Meskerem〜Pagume までの 13 ヶ月と 30/5/6 日構造。
- `year % 4 == 3` 判定による閏年計算。
- フォーマット指定子 `%E/%e`, `%Y/%y`, `%M/%m`, `%D/%d`, `%W/%w`, `%%`。
- エチオピア語の曜日 (`Ehud`, `Segno`, …) と英語曜日を `%W` / `%w` で出力。
- `strict` フラグによる曖昧一致検出と貪欲解釈の切り替え。
- `Date` や他暦 (`Gregorian`, `Julian`) とのシリアル共有による相互変換。

## 未実装 / 制限事項
- `from_serial_date` は epoch 以前 (ユリウス暦 AD 8-08-29 より前) をサポートしていないため、`BC` 変換や古代の年は例外になります。
- 月名 `%m` の入力は英語転写のみ対応 (Meskerem/Tikemet/…/Pagume)。別綴りやエチオピア語表記は未対応です。
- `%y` のパースは 2 桁固定、出力は 4 桁ゼロ埋めという仕様差があります。完全な往復性が必要な場合は `%Y` を使用してください。
- タイムゾーンや時間情報は扱いません。日単位のみの計算です。
- 実装は先発 (proleptic) のため、実際の採用史や閏年調整の微細な差異は考慮していません。

## 使い方
### `Date` 経由
```cpp
#include <Date.hpp>

// エチオピア暦 2015 年 Meskerem 1 日 (グレゴリオ暦 2022-09-11 頃)
toolbox::Date et(toolbox::ETHIOPIAN,
		toolbox::EthiopianCalendar::AD, 2015, 1, 1);

std::string ethio = et.to_string(toolbox::ETHIOPIAN, "%E%Y %m %D");
std::string greg = et.to_string(toolbox::GREGORIAN, "%Y-%m-%d");
```

### `EthiopianCalendar` を直接利用
```cpp
#include <calendar_system/EthiopianCalendar.hpp>

toolbox::EthiopianCalendar cal;

// 数値指定でシリアルへ
int serial = cal.to_serial_date(toolbox::EthiopianCalendar::AD,
															 2015, 13, 6);  // Pagume 6 (閏年)

// 月名フォーマットを含む文字列をパース
serial = cal.to_serial_date("AD2015 Meskerem 01",
														"%e%Y %m %d", true);

// シリアルからフォーマット済み文字列を取得
std::string out;
cal.from_serial_date(serial, out, "%E%Y-%M-%D (%W)");

// 個別要素を取得
int era, year, month, day;
cal.from_serial_date(serial, era, year, month, day);
```

## フォーマット仕様
| 指定子 | 入出力 | 説明 |
| --- | --- | --- |
| `%E` / `%e` | 双方向 | `BC`/`AD`。`%E` は "B.C."/"A.D.", `%e` は "BC"/"AD" |
| `%Y` | 双方向 | 年 (1 以上、可変長)。 |
| `%y` | 双方向 | 年の下位 2 桁を入力として想定 (出力は 4 桁ゼロ埋め)。 |
| `%M` | 双方向 | 月番号 (1〜13、可変長)。 |
| `%m` | 双方向 | 月名 (Meskerem, Tikemet, …, Pagume)。 |
| `%D` | 双方向 | 日 (可変長、数値)。 |
| `%d` | 双方向 | 日 (2 桁ゼロ埋め)。 |
| `%W` | 出力のみ | エチオピア語の曜日名 (Ehud, Segno, …, Kidame)。 |
| `%w` | 出力のみ | 英語の曜日名 (Sunday〜Saturday)。 |
| `%%` | 双方向 | リテラル `%`。 |

- フォーマットに含まれる空白・記号は完全一致が必要です。
- 可変長指定子を連続させると曖昧解釈が生じる可能性があるため、区切り文字の挿入を推奨します。

## 特殊処理と注意事項
- **Pagume の日数**: 平年は 5 日、`year % 4 == 3` の閏年は 6 日。`to_serial_date` はこの規則に違反する入力を拒否します。
- **epoch 以前の扱い**: `from_serial_date` は `serial < julian(AD 8-08-29)` で例外を投げます。エポックより前の表現が必要なら基準値を再定義する必要があります。
- **strict フラグ**: `%Y%m%d` のような曖昧フォーマットでは、`strict=true` で例外、`strict=false` で最初の成功解釈 (例: 月=11 / 日=12 など) を採用します。
- **月名と大文字小文字**: `%m` は英語月名をそのまま比較するため、入力は大文字始まりの定義どおりである必要があります (Meskerem, Tikemet ...)。
- **曜日出力の差**: `%W` はエチオピア語の曜日、`%w` は英語。用途に応じて使い分けてください。

## 改変・拡張のヒント

## テスト観点
- 閏年境界: 2011–2015 年 (Pagume 6 日) や `year % 4 == 3` の判定確認。
- 月名パース: 全 13 ヶ月の名前を含む入力を round-trip させる。
- strict フラグ: `"1111" + "%M1%D"` など曖昧フォーマットでの挙動検証。
- epoch 下限: AD 8-08-29 の直前/直後を `from_serial_date` で確認し、例外が発生することを確かめる。
- 曜日表記: `%W` がエチオピア語名、`%w` が英語名であることを確認。

## 参考
- [Ethiopian Calendar](https://www2s.biglobe.ne.jp/~yoss/tuki/EthiopianCalendar.html)