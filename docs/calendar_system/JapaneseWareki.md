# 和暦カレンダー実装仕様

## 和暦の特徴
- 元号 (元号記号) ごとに開始日・終了日が定義され、年は各元号の開始日を 1 年としてカウントします。
- 645 年の「大化」から現行の「令和」までをサポートし、南北朝期など複数の権威が存在した時期も網羅しています。
- 暦の計算自体はユリウス暦 (1582 年以前) とグレゴリオ暦 (1582 年 10 月 15 日以降) に委譲しており、和暦固有の月長・閏月処理は行いません。
- `JapaneseEra` 列挙で元号 ID を表現し、漢字表記との対応は `JapaneseEra.cpp` 内の `EraMetadata` テーブルで定義されています。

## 実装構成
### データソースと内部表現
- `JapaneseEra.hpp/.cpp` に全元号のメタデータ (`EraMetadata`) をハードコードしています。各項目は
  - 開始日/終了日 (ユリウス or グレゴリオ暦、BC/AD 対応)
  - 権威 (`ERA_AUTHORITY_STANDARD`/`SOUTHERN`/`NORTHERN`)
  - 終了日の有無 (`has_end`)
  - 漢字表記 (`kanji`)
 で構成されます。
- `JapaneseWarekiCalendar.cpp` では `load_era_ranges()` により `EraMetadata` を `EraRange` (開始/終了シリアル日、漢字名、南朝フラグ) に変換します。Julian/Gregorian カレンダーを使って即時にシリアル日に変換することで、実行時に CSV 等へ依存しません。

### 主なクラス
- `JapaneseWarekiCalendar` は `ICalendarSystem` を実装し、以下の機能を提供します。
  - 元号 + 年月日からシリアル日への変換 (`to_serial_date`)
  - 文字列からのパース (`to_serial_date(const std::string&, const char*, bool)`) ※書式指定子 `%E/%Y/%M/%D`
  - シリアル日から元号表記を含む日付を取得 (整数版と文字列版の `from_serial_date`)
  - 曜日計算 (`from_serial_date(int serial, int& day_of_week)`) はグレゴリオ暦に委譲
- `Date` クラスは `CalendarSystem::JAPANESE_WAREKI` を通じて上記カレンダーを利用します。

## 実装しているもの
- すべての歴史的元号 (南北朝を含む) と現行「令和」のオープンエンド対応。
- 元号境界の厳密な検証: `to_serial_date` は era 内かどうかを上限/下限でチェックし、範囲外は `std::out_of_range` を送出。
- 文字列パースでの漢字元号一致: `EraMetadata` の漢字を最大長一致で走査し、入力と最長一致した元号を採用。
- `strict` フラグ: `false` の場合は末尾の空白のみ許容、それ以外は完全一致を要求。
- 南北朝期の逆変換: 同じシリアル日に複数元号が重なる場合、南朝 (`is_southern`) を優先します。
- `Date` からの利用: `Date::to_string(JAPANESE_WAREKI, "%E%Y-%m-%d")` などで出力できます。

## 未実装 / 制限事項
- 漢数字・「元年」・ローマ字表記など、漢字以外の元号入力は未対応。年・月・日は半角数字のみ認識します。
- 旧暦の閏月や月初調整は行っていません。暦計算は純粋な Julian/Gregorian ベースであり、史実と異なる可能性があります。
- `load_era_ranges()` は呼び出し毎に全データを再生成します。現状キャッシュや遅延初期化はなく、大量変換時はパフォーマンスに影響します。
- 南北朝の選択は南朝優先に固定されており、北朝を選ぶ設定はありません。
- 未来の元号は `EraMetadata` に追加されるまで扱えません。`REIWA` の終了日は `has_end=false` で上限チェックをスキップしています。
- フォーマット指定子は `%E`, `%Y/%y`, `%M/%m`, `%D/%d`, `%%` のみ。有効桁幅やゼロ埋め指定は提供していません。

## 使い方
### `Date` 経由
```cpp
#include <Date.hpp>
#include <calendar_system/JapaneseEra.hpp>

// 令和 5 年 5 月 1 日を生成
toolbox::Date wareki(toolbox::JAPANESE_WAREKI,
    toolbox::REIWA, 5, 5, 1);

// 和暦でフォーマット
std::string s = wareki.to_string(toolbox::JAPANESE_WAREKI, "%E%Y年%m月%d日");
// s == "令和5年5月1日"

// グレゴリオ暦に変換
std::string iso = wareki.to_string(toolbox::GREGORIAN, "%Y-%m-%d");
```

### `JapaneseWarekiCalendar` を直接利用
```cpp
#include <calendar_system/JapaneseWarekiCalendar.hpp>

toolbox::JapaneseWarekiCalendar cal;

// 元号 + 数値からシリアル日に変換
int serial = cal.to_serial_date(toolbox::HEISEI, 31, 4, 30);

// 文字列パース (strict=false で末尾空白を許容)
serial = cal.to_serial_date("平成31年4月30日", "%E%Y年%M月%D日", false);

// シリアル日を和暦表記へ
std::string out;
cal.from_serial_date(serial, out, "%E%Y/%M/%D");

// 元号・年月日を個別に取得
int era, year, month, day;
cal.from_serial_date(serial, era, year, month, day);
```

## フォーマット仕様
| 指定子 | 説明 |
| --- | --- |
| `%E` / `%e` | `EraMetadata` の漢字名 (例: `令和`) |
| `%Y` / `%y` | 元号内の年 (十進数、1 始まり) |
| `%M` / `%m` | 月 (十進数、ゼロ埋めなし) |
| `%D` / `%d` | 日 (十進数、ゼロ埋めなし) |
| `%%` | 文字 `%` をそのまま出力/期待 |

- 入力パース時はリテラル文字が完全一致する必要があります。`strict=false` でもリテラル途中のゆらぎは認めません。
- 出力時はゼロ埋め・桁制御がないため、必要であれば呼び出し側で `std::format` や `std::ostringstream` を組み合わせてください。

## 特殊処理と注意事項
- **年は 1 以上のみ**: 0 年や負値を渡すと `std::out_of_range` になります。
- **境界チェック**: 元号の開始日前/終了日後を指定すると例外が送出されます。終了日が未定義の元号は上限チェックをスキップします。
- **南北朝期の逆引き**: 同一シリアル日を複数元号がカバーする場合、`from_serial_date` は自動的に南朝を選択します。北朝を選ぶ API は未提供です。
- **暦法切り替え**: 1582-10-15 を境にシリアル→暦変換で Julian/Gregorian を切り替えています。連続するシリアル番号を前提にしているため、実歴史のグレゴリオ暦導入ギャップ (10 日間スキップ) には注意してください。
- **例外メッセージ**: `from_serial_date` で対応元号が見つからない場合、直前/直後の元号名を付けた詳細な `std::out_of_range` を返します。ユーザー向けフィードバックにそのまま利用可能です。
- **データ初期化コスト**: `load_era_ranges()` は毎回 `EraMetadata` 全件を走査します。大量に呼び出す場合は、ラッパーでキャッシュするか、`static` 変数化する最適化を検討してください。

## 改変・拡張のヒント
- **新元号の追加**: `JapaneseEra` 列挙に値を追加し、`EraMetadata` に開始日・終了日 (または `has_end=false`)・漢字名・権威を登録してください。`EraMetadataSizeCheck` が配列サイズと列挙値を検証します。
- **別表記や略称への対応**: `to_serial_date(const std::string&, ...)` は `EraMetadata` の漢字をそのまま使うため、別名やローマ字を許可したい場合はマッピングテーブルを追加する必要があります。

## テスト観点
- 元号境界前後 (例: 明治 45 年 7 月 29 日/大正 1 年 7 月 30 日) の往復変換。
- 南北朝期のシリアル→和暦変換で南朝が選ばれているか。
- フォーマット/パースで `%` エスケープや `strict` フラグを含むケース。
- `REIWA` のように終了日未定の元号で上限チェックが行われないこと。
