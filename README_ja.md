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
不正な引数、各処理の時間、正常完了、burnoutの時刻、cooldown、実際のdongle待ち行列でのFIFO/EDF、同じdeadlineでの優先順位、要求の取消、ヒープからの削除、資源の取得順、待ち時間の連鎖の防止を検査します。
メモリ確保、同期オブジェクトの初期化、スレッド作成の各段階で失敗させ、資源を解放できることも確認します。
検証用のソースと実行ファイルは一時ディレクトリに作成し、テスト後に削除します。

## 待ちと停止への対策（Blocking cases handled）

- **デッドロックと待ち時間の連鎖**：リングの隣接関係に合わせて、dongleに取得順を示すrankを割り当てます。
  奇数IDはrank 0、偶数IDはrank 1とし、dongleの個数が奇数なら最後のdongleだけrank 2にします。
  coderはrankの小さいdongleから取得します。
  隣接するdongleのrankは異なるため、最初のdongleを保持して次を待つ依存関係は最大2辺です。
  これにより、単純なID順取得で起こり得る長い待ち時間の連鎖を防ぎます。
  相互排他と、資源を保持したまま別の資源を待つ動作は残りますが、この取得順によってCoffmanのデッドロック条件のうち循環待ちを成立させません。
- **飢餓と優先順位**：各dongleは、取得可能になったときにヒープの先頭の要求だけを許可します。
  FIFOはそのdongleへの要求の到着順、EDFは`last_compile_start + time_to_burnout`、到着順、coder IDの順で比較します。
  待機中のcoderのdeadlineは変わらず、待機中に新しいcompile開始時刻で更新されることもありません。
  EDFは処理が間に合う設定で古い期限を優先しますが、OSのスレッドスケジューリングや処理が間に合わない設定は動作時刻に影響します。
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

各dongleの`pthread_mutex_t`は、所有者、cooldownの期限、到着順の連番、要求ヒープを保護します。
要求するのは隣接する2人のcoderだけなので、ヒープの容量は2です。
`pthread_cond_timedwait`は待機中にmutexを解放します。
起床するたびに所有者、ヒープの先頭、cooldown、シミュレーションの停止フラグを確認するため、条件を満たさない起床によって所有権を渡すことはありません。
dongleの解放時には、待機中のスレッドへbroadcastします。
1ミリ秒のタイムアウトも設け、専用の通知スレッドを追加せずにcooldownの満了や停止を確認します。

`stop_mutex`は、開始待ちの状態、停止フラグ、各coderの直前のcompile開始時刻、compile完了回数を保護します。
たとえば、compile開始時刻の更新と、それまでの期限の確認を同じクリティカルセクションで行います。
これにより、監視スレッドが更新前の時刻を使ってburnoutを確定することを防ぎます。
coderのスレッドは他のcoderのdeadlineを直接読みません。
資源の取得順は、記録済みの要求を使って決めます。

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
