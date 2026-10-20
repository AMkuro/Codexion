*This project has been created as part of the 42 curriculum by miwasaki*

# Codexion

提出用の正本は[README.md](README.md)です。
このファイルはその日本語訳です。

## 概要（Description）

Codexionは、USB dongleを取り合うcoderの動作をC言語でシミュレーションするプログラムです。
課題仕様書（Subject）v1.3に従い、各coderがPOSIXスレッドでcompile、debug、refactorを繰り返します。
compileには左右のdongleが両方必要で、専用の監視スレッドがburnoutの期限を確認します。

各dongleは自作の二分ヒープを使い、FIFOまたはEDFで競合する取得要求の優先順位を決めます。
シミュレーションは、いずれかのcoderがburnoutするか、全員が指定回数のcompileを完了すると終了します。
指定回数に達したcoderも、全員が達成するまでは通常の動作を続け、burnoutの監視対象になります。

## 使い方（Instructions）

Cコンパイラ、Make、POSIXスレッドが必要です。
libftや追加の実行時ライブラリは使用しません。

```sh
make
./codexion 5 1000 20 20 20 3 10 edf
```

次の8個の引数を、表の順番ですべて指定します。

| 引数 | 意味 |
| --- | --- |
| `number_of_coders` | coderのスレッド数とdongleの個数。正の整数 |
| `time_to_burnout` | シミュレーション開始または直前のcompile開始から、次のcompileを開始するまでの期限。単位はミリ秒 |
| `time_to_compile` | 2個のdongleを保持してcompileする時間。単位はミリ秒 |
| `time_to_debug` | debugする時間。単位はミリ秒 |
| `time_to_refactor` | dongleの取得を再び要求する前に、refactorに使う時間。単位はミリ秒 |
| `number_of_compiles_required` | 各coderに必要なcompileの完了回数 |
| `dongle_cooldown` | 解放したdongleを再取得できない時間。単位はミリ秒。0も指定可能 |
| `scheduler` | `fifo`または`edf`を指定 |

`time_to_burnout`、`time_to_compile`、`time_to_debug`、`time_to_refactor`は正の値を指定します。
数値引数には十進数字だけを使い、符号、空白、小数、空文字列、オーバーフローする値は拒否します。
個数と回数の上限は`INT_MAX`です。
時刻と期限の加算に余裕を残すため、時間の上限は`LONG_MAX / 2`にしています。
配列サイズのオーバーフローも拒否し、メモリ確保やスレッド作成の失敗はエラーとして扱います。

coderが1人の場合は唯一のdongleを取得しますが、2個そろえられないためcompileできず、burnoutします。
資源の競合、各処理の時間、cooldownを考慮しても期限に間に合わない設定では、複数人でもburnoutします。
EDFを選んでも、処理が間に合わない設定での完了は保証できません。

ログには、開始からの経過ミリ秒、coderのID、Subjectで定義された5種類の状態のいずれかを出力します。
burnoutの行が最後のログになります。
正常完了とburnoutの終了コードは0です。
不正な入力、初期化の失敗、スレッド作成の失敗では0以外を返します。

```sh
make clean    # objectファイルと依存関係ファイルを削除
make fclean   # 実行ファイルも削除
make re       # 最初からビルドし直す
python3 tests/test_codexion.py
norminette includes srcs
```

回帰テストにはPython 3とCコンパイラが必要です。
不正な引数、各処理の時間、正常完了、burnoutの時刻、cooldown、実際のdongle待ち行列でのFIFO/EDF、同じdeadlineでの優先順位、要求の取消、ヒープからの削除、EDFのtie-break、EDFでの待ち時間の連鎖と保持したままの待機の防止を検査します。
メモリ確保、同期オブジェクトの初期化、スレッド作成の各段階で失敗させ、資源を解放できることも確認します。
検証用のソースと実行ファイルは一時ディレクトリに作成し、テスト後に削除します。

## 待ちと停止への対策（Blocking cases handled）

- **デッドロックと保持したままの待機**：coderは2本のdongleを一度にまとめて取得し、片方だけを取ることはありません。
  取得を試みるたびに自分の2本のmutexをID順にロックし、待機する前に両方とも解放します。
  ロック順が全体で1つに決まっていて、mutexを保持したまま待たないため、Coffmanのデッドロック条件のうち循環待ちは成立しません。
- **飢餓と優先順位**：coderごとに1つの要求を作り、両方のdongleのヒープに入れます。
  そのため、どのdongleも自分を必要とするcoderを全員把握しています。
  dongleはヒープの先頭にしか渡さず、coderは両方のヒープで先頭になったときだけcompileします。
  FIFOはシミュレーション全体で共通の到着順で比較するので、2つのヒープで順番が食い違いません。
  EDFは`last_compile_start + time_to_burnout`で比較します。
  deadlineが同じときは、compile完了回数が少ないほう、リング上のrankが小さいほう、到着順、coder IDの順に優先します。
  rankは奇数IDが0、偶数IDが1で、coderが奇数人なら最後のcoderだけ2にするので、隣り合うcoderどうしは必ず違う値になります。
  開始直後は全員のdeadlineが同じですが、rankによって最初の割り当てがリング上に散らばり、隣どうしが順番待ちで連なることを防ぎます。
  待機中のcoderのdeadlineは変わらず、待機中に新しいcompile開始時刻で更新されることもありません。
- **FIFOの限界**：FIFOは到着順だけで決めるので、待っている先頭のcoderが後ろの隣を止め、その隣がさらに次の隣を止めることがあります。
  最悪の場合、リング全体で1人ずつしかcompileできません。
  Subjectがlivenessを求めているのはEDFだけです。
- **間に合う設定の限界**：空いたdongleはすぐに使います。
  1回のcompileでdongleは`time_to_compile + dongle_cooldown`の間ふさがり、同時にcompileできるのは`number_of_coders / 2`人までです。
  そのため全員が1回ずつcompileするには、奇数人のリングで3巡、偶数人のリングで2巡かかります。
  奇数人なら`3 * (time_to_compile + dongle_cooldown) < time_to_burnout`、偶数人なら`2 *`が必要条件です。
  `5 3000 200 200 200 10 800`はちょうど`3 * 1000 = 3000`で境目にあり、burnoutします。
  開始を0、500、1000、1500、2000ミリ秒のようにわざとずらせば間に合いますが、空いているdongleを使わずにおく判断はFIFOでもEDFでもありません。
  境目に近い設定では、OSのスケジューリングによる数ミリ秒の遅れで結果が変わります。
- **Cooldown**：dongleの解放時に、そのmutexを保持して`release_time + dongle_cooldown`を記録します。
  最優先の要求でも、この時刻までは取得できません。
- **Burnout**：全coderの開始時刻をそろえ、監視スレッドが500マイクロ秒ごとに期限を確認します。
  compileの開始を許可するときと完了時にも、同じ状態ロックで期限切れを確認します。
  期限切れ後の開始時刻の更新や、監視スレッドが古い開始時刻で判定する競合を防ぎます。
  個別に必要回数へ達しても監視は続きます。
- **完了と取消**：全員の必要回数が満たされた時点で、終了状態を確定します。
  待機中のスレッドは、戻る前にスタック上の要求をヒープから除去します。
  取得済みのdongleも終了時に解放します。
- **ログ**：出力ロックと状態ロックを保持したまま、終了状態とburnoutログを確定します。
  終了状態になった後は通常のログを出しません。
- **初期化途中の失敗**：初期化段階を記録し、作成済みのmutex、条件変数、ヒープだけを破棄して、2つの配列を解放します。
  スレッド作成に失敗した場合は、それまでに作成したスレッドを停止してjoinします。

テストでは、burnoutを10ミリ秒以内に報告する要件を確認します。
プロセスの一時停止、実時刻の変更、標準出力への書き込みの停止によって、この時間を守れない場合があります。
時刻の検査は、通常のOSスケジューリングと、書き込みを受け付ける出力先を前提とします。

## スレッドの同期（Thread synchronization mechanisms）

各dongleの`pthread_mutex_t`は、所有者、cooldownの期限、要求ヒープを保護します。
要求するのは隣接する2人のcoderだけなので、ヒープの容量は2です。
dongleを取れなかったcoderは2本のmutexを両方とも解放し、`usleep(500)`で500マイクロ秒待ってから確認し直します。
確認のたびに2本のロックを取り直し、所有者、ヒープの先頭、cooldown、シミュレーションの停止フラグを見ます。
すべての条件がそろったときだけ所有権を受け取ります。
cooldownの満了や停止にも、この定期的な確認で気づくので、スレッド間の通知はいりません。

`stop_mutex`は、開始待ちの状態、停止フラグ、要求の到着順、各coderの直前のcompile開始時刻、compile完了回数を保護します。
要求を作るときは、このロックの下でdeadlineとtie-breakに使う値を写し取ります。
たとえば、compile開始時刻の更新と、それまでの期限の確認を同じクリティカルセクションで行います。
これにより、監視スレッドが更新前の時刻を使ってburnoutを確定することを防ぎます。
coderのスレッドは他のcoderのdeadlineを直接読みません。
資源の割り当ては、ヒープに入っている要求を使って決めます。

`log_mutex`は出力を直列化し、状態変更に伴うログの判定も保護します。
両方のロックが必要なときは、`log_mutex`、`stop_mutex`の順に取得します。
dongleの取得処理はdongleのmutexを保持したまま`stop_mutex`を取得する場合がありますが、状態管理とログの処理からdongleのmutexを取得することはありません。
各処理の待機やスレッドのjoin中は、状態ロックを保持しません。

条件変数`start_cond`によって、全coderと監視スレッドはスレッド作成が成功するまで開始を待ちます。
mainが共通の開始時刻を設定してbroadcastすると、各スレッドが動き始めます。
作成に失敗した場合は停止フラグを設定し、開始待ちを解除します。
これにより、途中まで作成したスレッドも終了してjoinできます。

## 参考資料（Resources）

- Codexion Subject v1.3：FIFO/EDF、deadline、cooldown、ログ、READMEを含む課題要件。
- [POSIXのmutex操作](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_mutex_lock.html)
- [POSIXの条件変数による待機](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_cond_timedwait.html)
- [POSIXのスレッド作成](https://pubs.opengroup.org/onlinepubs/9799919799/functions/pthread_create.html)
- [42公式Norminette](https://github.com/42School/norminette)
- Cormen, Leiserson, Rivest, Stein, *Introduction to Algorithms*：二分ヒープと優先度付きキュー。
- Coffman, Elphick, Shoshani, *System Deadlocks*：デッドロックの成立条件。

今回の修正では、既存コードとSubjectの照合、パーサー、スケジューリング、同期処理、資源解放の修正、回帰テストとREADMEの作成、修正後のレビューにAIを使用しました。
