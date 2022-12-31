# Multi-Cycle Pipelined MIPS
<html>

<head>
<meta http-equiv=Content-Type content="text/html; charset=utf-8">
<meta name=Generator content="Microsoft Word 15 (filtered)">
</head>

<div class=WordSection1>

<p class=MsoNormal style='text-autospace:ideograph-numeric ideograph-other;
word-break:keep-all'><b><span lang=EN-US style='font-size:16.0pt;line-height:
107%'>&nbsp;</span></b></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314822"><span lang=EN-US style='font-size:12.0pt;
line-height:107%'>MIPS ISA</span></a></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>ISA(Instruction
Set Architecture)</span><span lang=KO>란 </span><span lang=EN-US>CPU</span><span
lang=KO>가 소프트웨어로 어떻게 제어될지를 정의해 놓은 추상화 모델의 일종이다</span><span lang=EN-US>. ISA</span><span
lang=KO>는 하드웨어와 소프트웨어 사이의 인터페이스 역할을 하며</span><span lang=EN-US>, </span><span
lang=KO>프로세서가 수행할 수 있는 기능과 처리 방법을 모두 명시한다</span><span lang=EN-US>. </span><span
lang=KO>본문에서는 </span><span lang=EN-US>MIPS-32 ISA</span><span lang=KO>중 정수 부분을 다룬다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>MIPS-32 ISA</span><span
lang=KO>의 각 명령은 </span><span lang=EN-US>4</span><span lang=KO>바이트</span><span
lang=EN-US>, </span><span lang=KO>즉 </span><span lang=EN-US>32</span><span
lang=KO>비트 크기의 이진수로 표현하고 하나의 명령어에 하나의 연산을 수행한다</span><span lang=EN-US>. MIPS </span><span
lang=KO>프로세서는 </span><span lang=EN-US>4</span><span lang=KO>바이트 크기의 </span><span
lang=EN-US>General Purpose Register 32</span><span lang=KO>개를 갖고 있고</span><span
lang=EN-US>, </span><span lang=KO>각각 </span><span lang=EN-US>$0 ~ $31</span><span
lang=KO>로 표현한다</span><span lang=EN-US>. </span><span lang=KO>이 중 </span><span
lang=EN-US>$0</span><span lang=KO>은 항상 </span><span lang=EN-US>0</span><span
lang=KO>의 값을 가지며</span><span lang=EN-US>, $31</span><span lang=KO>은 프로그램 실행 도중 </span><span
lang=EN-US>jal </span><span lang=KO>등의 명령어 수행으로 다른 곳으로 점프했을 때 점프 명령어의 주소 </span><span
lang=EN-US>+ 4 </span><span lang=KO>혹은 </span><span lang=EN-US>+ 8 </span><span
lang=KO>을 저장하는 레지스터이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-align:center;text-indent:10.0pt'><span lang=EN-US><img width=451
height=118 id="그림 2" src="README.fld/image001.jpg"
alt="테이블이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>기본적인 명령어로는 </span><span
lang=EN-US>R</span><span lang=KO>형식</span><span lang=EN-US>, I</span><span
lang=KO>형식</span><span lang=EN-US>, J</span><span lang=KO>형식이 있다</span><span
lang=EN-US>. opcode</span><span lang=KO>는 세 형식 모두 상위 </span><span lang=EN-US>6</span><span
lang=KO>비트로 동일하다</span><span lang=EN-US>. R</span><span lang=KO>형식은 </span><span
lang=EN-US>opcode</span><span lang=KO>가 </span><span lang=EN-US>0</span><span
lang=KO>인 명령어로</span><span lang=EN-US>, 5</span><span lang=KO>비트 크기의 레지스터 번호 </span><span
lang=EN-US>rs, rt, rd</span><span lang=KO>와 명령어가 </span><span lang=EN-US>sll,
srl</span><span lang=KO>일 경우 사용할 </span><span lang=EN-US>shamt, </span><span
lang=KO>명령어를 구분할</span><span lang=EN-US> 6</span><span lang=KO>비트 크기의 </span><span
lang=EN-US>funct</span><span lang=KO>를 갖는다</span><span lang=EN-US>. R</span><span
lang=KO>형식은 대부분의 경우 </span><span lang=EN-US>R[rs]</span><span lang=KO>와 </span><span
lang=EN-US>R[rt]</span><span lang=KO>를 연산한 결과를 </span><span lang=EN-US>R[rd]</span><span
lang=KO>에 저장하는 명령어이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>I</span><span
lang=KO>형식은 </span><span lang=EN-US>5</span><span lang=KO>비트 크기의 레지스터 번호 </span><span
lang=EN-US>rs, rt</span><span lang=KO>와 </span><span lang=EN-US>16</span><span
lang=KO>비트 크기의 </span><span lang=EN-US>immediate</span><span lang=KO>를 갖는다</span><span
lang=EN-US>. I</span><span lang=KO>형식 명령어는 대부분의 경우 </span><span lang=EN-US>immediate</span><span
lang=KO>를 </span><span lang=EN-US>opcode</span><span lang=KO>에 따라 부호확장이나 제로확장 한
후 </span><span lang=EN-US>R[rs]</span><span lang=KO>와의 연산을 거쳐 </span><span
lang=EN-US>R[rt]</span><span lang=KO>에 저장하는 명령어이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>J</span><span
lang=KO>형식은 </span><span lang=EN-US>26</span><span lang=KO>비트 크기의 </span><span
lang=EN-US>address</span><span lang=KO>를 갖는다</span><span lang=EN-US>. address</span><span
lang=KO>는 왼쪽으로 </span><span lang=EN-US>2</span><span lang=KO>번 </span><span
lang=EN-US>shift</span><span lang=KO>된 후 </span><span lang=EN-US>PC</span><span
lang=KO>값의 상위 </span><span lang=EN-US>4</span><span lang=KO>비트를 </span><span
lang=EN-US>address</span><span lang=KO>의 상위비트 앞에 연결함으로써 </span><span
lang=EN-US>jump target</span><span lang=KO>주소로 계산된다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>필자가 작성한 프로그램에서 구현한 명령어는
다음과 같다</span><span lang=EN-US>.</span></p>

<p class=MsoListParagraph style='margin-left:28.0pt;text-indent:-18.0pt'><span
lang=EN-US>-<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span lang=EN-US>R</span><span lang=KO>형식 </span><span
lang=EN-US>: add, addu, and, jalr, jr, nor, or, slt, sltu, sll, srl, sub, subu</span></p>

<p class=MsoListParagraph style='margin-left:28.0pt;text-indent:-18.0pt'><span
lang=EN-US>-<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span lang=EN-US>I</span><span lang=KO>형식 </span><span
lang=EN-US>: addi, addiu, andi, beq, bne, lui, lw, ori, slti, sltiu, sw</span></p>

<p class=MsoListParagraph style='margin-left:28.0pt;text-indent:-18.0pt'><span
lang=EN-US>-<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span lang=EN-US>J</span><span lang=KO>형식 </span><span
lang=EN-US>: j, jal</span></p>

<h2><a name="_Toc104314823"><span lang=EN-US style='font-size:12.0pt;
line-height:107%'>Multi-Cycle Pipelined MIPS processor</span></a></h2>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=451 height=163 id="그림 10" src="README.fld/image002.jpg"
alt="텍스트, 시계이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span><a href="#_edn1"
name="_ednref1" title=""><span class=MsoEndnoteReference><span lang=EN-US><span
class=MsoEndnoteReference><span lang=EN-US style='font-size:10.0pt;line-height:
107%;font-family:"맑은 고딕",sans-serif'>[i]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(tistory, n.d.)</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>Multi-Cycle</span><span
lang=KO>은 하나의 명령어를 여러 단계로 나눠서 각 단계를 한 클럭 사이클마다 처리하는 방법이다</span><span
lang=EN-US>. </span><span lang=KO>따라서 명령어 당 사이클 수</span><span lang=EN-US>(CPI)</span><span
lang=KO>는 나눈 단계의 개수이다</span><span lang=EN-US>. </span><span lang=KO>파이프라이닝은 여기서
파이프라인 레지스터를 도입한 방식이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=451 height=163 id="그림 20" src="README.fld/image003.jpg"></span><a
href="#_edn2" name="_ednref2" title=""><span class=MsoEndnoteReference><span
lang=EN-US><span class=MsoEndnoteReference><span lang=EN-US style='font-size:
10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[ii]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(tistory, n.d.)</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>각 단계 사이에 파이프라인 레지스터를
놓아서 클럭 신호가 상향 엣지일때 파이프라인에 저장된 데이터를 다음 단계로 </span><span lang=EN-US>fetch</span><span
lang=KO>하고 하향 엣지일때 이전 단계에서 나온 값이 파이프라인에 저장된다</span><span lang=EN-US>. 5</span><span
lang=KO>단계로 명령어 실행 과정을 나눴을 때 파이프라인을 통해 한 클럭 사이클당 최대 </span><span lang=EN-US>5</span><span
lang=KO>개의 명령어를 실행 할 수 있으므로 명령어 처리율을 크게 향상시킬 수 있다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314824"><span lang=EN-US style='font-size:12.0pt;
line-height:107%'>Single-Cycle</span></a><span lang=KO style='font-size:12.0pt;
line-height:107%'>과의 차이</span></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>명령어를</span><span
lang=EN-US> Single-Cycle</span><span lang=KO>보다 </span><span lang=EN-US>Clock
Cycle Time</span><span lang=KO>을 작게 정해서 </span><span lang=EN-US>Multi-Cycle</span><span
lang=KO>로 수행하면</span><span lang=EN-US> CPI</span><span lang=KO>는 증가하게 되지만 </span><span
lang=EN-US>R</span><span lang=KO>형식 명령어나 </span><span lang=EN-US>j </span><span
lang=KO>명령어와 같이 실행되는 명령어가 필요한 단계만 거쳐가게 되므로 총 </span><span lang=EN-US>Clock
Cycle</span><span lang=KO>은 감소하게 된다</span><span lang=EN-US>. </span><span
lang=KO>따라서 전체적인 성능에 향상이 이루어진다</span><span lang=EN-US>. </span><span lang=KO>여기에
파이프라인을 도입하면 모든 명령어가 모든 단계를 거쳐가게 되지만</span><span lang=EN-US>, </span><span
lang=KO>한번에 실행할 수 있는 명령어가 많아지고</span><span lang=EN-US>, </span><span lang=KO>매 </span><span
lang=EN-US>Clock Cycle </span><span lang=KO>마다 하나의 명령어가 실행완료 되므로 파이프라인의 </span><span
lang=EN-US>CPI</span><span lang=KO>는 이상적일 때 </span><span lang=EN-US>1</span><span
lang=KO>이 된다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>하지만 앞에서 </span><span
lang=EN-US>“</span><span lang=KO>이상적일 때</span><span lang=EN-US>” </span><span
lang=KO>라 말했듯이 파이프라이닝에 결점이 없는 것은 아니다</span><span lang=EN-US>. </span><span
lang=KO>파이프라인을 도입하게 되면 </span><span lang=EN-US>Single Cycle</span><span
lang=KO>에는 존재하지 않던 </span><span lang=EN-US>Data dependency</span><span lang=KO>와
</span><span lang=EN-US>Control dependency</span><span lang=KO>가 생긴다</span><span
lang=EN-US>. </span><span lang=KO>이런 </span><span lang=EN-US>hazard</span><span
lang=KO>가 발생하면 </span><span lang=EN-US>CPI</span><span lang=KO>가 </span><span
lang=EN-US>1</span><span lang=KO>보다 높아지는 경우가 생길 수도 있고</span><span lang=EN-US>, </span><span
lang=KO>결과값이 달라질 수도 있다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314825"><span lang=EN-US style='font-size:12.0pt;
line-height:107%'>Data dependency</span></a></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>Data dependency</span><span
lang=KO>의 종류는 다음 표와 같다</span><span lang=EN-US>.</span></p>

<table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0
 style='border-collapse:collapse;border:none'>
 <tr>
  <td width=200 valign=top style='width:150.25pt;border:solid windowtext 1.0pt;
  background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='color:black'>구분</span></p>
  </td>
  <td width=200 valign=top style='width:150.25pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='color:black'>내용</span></p>
  </td>
  <td width=200 valign=top style='width:150.3pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='color:black'>예시</span></p>
  </td>
 </tr>
 <tr>
  <td width=200 valign=top style='width:150.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>RAW (Read After Write)</span></p>
  </td>
  <td width=200 valign=top style='width:150.25pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO>이전 명령이 저장한 연산 결과를 후속 명령이 읽으려고 할 때</span></p>
  </td>
  <td width=200 valign=top style='width:150.3pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>add <b><span style='color:red'>r1</span></b> &lt;- r2, r3</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>add r4 &lt;- <b><span style='color:red'>r1</span></b>, r5</span></p>
  </td>
 </tr>
 <tr>
  <td width=200 valign=top style='width:150.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>WAR (Write After Read)</span></p>
  </td>
  <td width=200 valign=top style='width:150.25pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO>이전 명령이 값을 읽기 전에 후속 명령이 값을 쓰는 경우</span></p>
  </td>
  <td width=200 valign=top style='width:150.3pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>add r1 &lt;- <b><span style='color:red'>r2</span></b>, r3</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>sub <b><span style='color:red'>r2</span></b> &lt;- r4, r1</span></p>
  </td>
 </tr>
 <tr>
  <td width=200 valign=top style='width:150.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>WAW (Write After Write)</span></p>
  </td>
  <td width=200 valign=top style='width:150.25pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO>이전 명령이 값을 쓰기 전에 후속 명령이 값을 쓰는 경우</span></p>
  </td>
  <td width=200 valign=top style='width:150.3pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>add <b><span style='color:red'>r1</span></b> &lt;- r2, r3</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>sub r2 &lt;- r4, r1</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US>or <b><span style='color:red'>r1</span></b> &lt;- r6, r3</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>WAW, WAR</span><span
lang=KO>의 경우 단일 파이프라인에서는 발생하지 않기 때문에 우리가 고려해야 할 것은 </span><span lang=EN-US>RAW</span><span
lang=KO>뿐이다</span><span lang=EN-US>. RAW</span><span lang=KO>는 같은 레지스터에 쓰고 읽는 명령어들
사이 거리가 </span><span lang=EN-US>3 </span><span lang=KO>이하일 때 발생한다</span><span
lang=EN-US>. RAW hazard</span><span lang=KO>를 해결하기 위하여 필자가 작성한 프로그램에서 구현한 방법은 </span><span
lang=EN-US>Forwarding</span><span lang=KO>과 </span><span lang=EN-US>Stalling</span><span
lang=KO>이다</span><span lang=EN-US>.</span></p>

<h2><a name="_Toc104314826"><span lang=EN-US style='font-size:12.0pt;
line-height:107%'>Control dependency</span></a></h2>

<p class=MsoNormal align=center style='text-align:center'><span lang=EN-US><img
width=270 height=175 id="그림 21" src="README.fld/image004.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center'><a href="#_edn3"
name="_ednref3" title=""><span class=MsoEndnoteReference><span lang=EN-US><span
class=MsoEndnoteReference><span lang=EN-US style='font-size:10.0pt;line-height:
107%;font-family:"맑은 고딕",sans-serif'>[iii]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>위 그림처럼 </span><span
lang=EN-US>Fetch(IF) </span><span lang=KO>단계에서 명령어가 </span><span lang=EN-US>fetch</span><span
lang=KO>된 후 </span><span lang=EN-US>Decode(ID) </span><span lang=KO>단계에서 어떤 명령어인지
식별하는데</span><span lang=EN-US>, </span><span lang=KO>이 때 식별된 명령어가 </span><span
lang=EN-US>conditional jump(beq, bne)</span><span lang=KO>인 경우 </span><span
lang=EN-US>branch target address</span><span lang=KO>는 </span><span lang=EN-US>Execution(EX)
</span><span lang=KO>단계에서 계산되어 </span><span lang=EN-US>Memory access(MEM) </span><span
lang=KO>단계에 분기 여부가 결정나게 된다</span><span lang=EN-US>. </span><span lang=KO>이를 위해선
분기 명령어가 </span><span lang=EN-US>MEM</span><span lang=KO>단계까지 가는 동안인 </span><span
lang=EN-US>3cycle</span><span lang=KO>동안 </span><span lang=EN-US>NOP</span><span
lang=KO>가 수행되어야 한다</span><span lang=EN-US>. </span><span lang=KO>그러나 이는 성능에 있어서
큰 손실을 야기한다</span><span lang=EN-US>. </span><span lang=KO>필자가 작성한 프로그램에선 이러한 손실을</span><span
lang=EN-US> 1cycle</span><span lang=KO>로 줄이기 위해 </span><span lang=EN-US>ID</span><span
lang=KO>단계에서 두 레지스터 값을 </span><span lang=EN-US>comparator</span><span lang=KO>로
비교함과 동시에 </span><span lang=EN-US>branch target address</span><span lang=KO>를 계산하도록
구현하였다</span><span lang=EN-US>. </span><span lang=KO>단</span><span lang=EN-US>, branch
</span><span lang=KO>연산을 </span><span lang=EN-US>ID</span><span lang=KO>단계에서 완료되도록
하면 </span><span lang=EN-US>Data dependency</span><span lang=KO>가 추가적으로 일어나게 된다</span><span
lang=EN-US>. </span><span lang=KO>이에 대한 </span><span lang=EN-US>Forwarding </span><span
lang=KO>및 </span><span lang=EN-US>Stalling</span><span lang=KO>은 </span><span
lang=EN-US>Implementation-</span><span lang=KO>전방전달 유닛에서 설명한다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314827"><span lang=KO style='font-size:12.0pt;line-height:
107%'>분기 예측</span></a></h2>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=227 height=77 id="그림 5" src="README.fld/image005.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=250 height=163 id="그림 6" src="README.fld/image006.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=267 height=162 id="그림 9" src="README.fld/image007.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center'><a href="#_edn4"
name="_ednref4" title=""><span class=MsoEndnoteReference><span lang=EN-US><span
class=MsoEndnoteReference><span lang=EN-US style='font-size:10.0pt;line-height:
107%;font-family:"맑은 고딕",sans-serif'>[iv]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>분기 연산을 </span><span
lang=EN-US>ID</span><span lang=KO>로 옮김으로써 </span><span lang=EN-US>3cycle</span><span
lang=KO>이던 </span><span lang=EN-US>NOP</span><span lang=KO>를 </span><span
lang=EN-US>1cycle</span><span lang=KO>로 줄였지만</span><span lang=EN-US>, </span><span
lang=KO>여전히 성능손실은 남아있다</span><span lang=EN-US>. </span><span lang=KO>이를 최대한 없애기
위해선 분기 예측이 필요하다</span><span lang=EN-US>. IF</span><span lang=KO>단계에서</span><span
lang=EN-US> Instruction memory</span><span lang=KO>로 </span><span lang=EN-US>PC</span><span
lang=KO>값을 보낸 직후 분기 예측기를 통해 </span><span lang=EN-US>PC + 4</span><span lang=KO>와
</span><span lang=EN-US>branch target address </span><span lang=KO>중 다음 </span><span
lang=EN-US>PC</span><span lang=KO>값을 정하고</span><span lang=EN-US>, </span><span
lang=KO>다음 </span><span lang=EN-US>Clock Cycle</span><span lang=KO>에서 예측이 맞으면 </span><span
lang=EN-US>NOP</span><span lang=KO>가 하나 줄어들어 이득을 볼 수 있고</span><span lang=EN-US>,
</span><span lang=KO>만약 틀리면 그 때 </span><span lang=EN-US>IF/ID </span><span
lang=KO>파이프라인의 명령어를 </span><span lang=EN-US>0</span><span lang=KO>으로 변경하여 </span><span
lang=EN-US>NOP</span><span lang=KO>를 추가하면 된다</span><span lang=EN-US>. </span><span
lang=KO>위 사진과 같이 </span><span lang=EN-US>Clock 3</span><span lang=KO>에서 </span><span
lang=EN-US>not taken</span><span lang=KO>이라 예측을 하고 </span><span lang=EN-US>0x44
</span><span lang=KO>주소의 명령어를 </span><span lang=EN-US>fetch</span><span
lang=KO>하였지만</span><span lang=EN-US>, Clock 4</span><span lang=KO>에서 </span><span
lang=EN-US>Decode </span><span lang=KO>후 </span><span lang=EN-US>beq</span><span
lang=KO>가 </span><span lang=EN-US>taken</span><span lang=KO>이라는 것을 알게 되었으므로 </span><span
lang=EN-US>Control unit</span><span lang=KO>에서 </span><span lang=EN-US>IF.Flush</span><span
lang=KO>신호를 인가하여 </span><span lang=EN-US>IF/ID </span><span lang=KO>파이프라인의 명령어 부분을
</span><span lang=EN-US>0</span><span lang=KO>으로 만들어 </span><span lang=EN-US>NOP</span><span
lang=KO>를 추가하는 것이다</span><span lang=EN-US>.</span></p>

<b><span lang=EN-US style='font-size:10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'><br
clear=all style='page-break-before:always'>
</span></b>

<p class=MsoNormal style='text-autospace:ideograph-numeric ideograph-other;
word-break:keep-all'><b><span lang=EN-US style='font-size:28.0pt;line-height:
107%;color:#404040;letter-spacing:-.5pt'>&nbsp;</span></b></p>

<h1><a name="_Toc104314828"><b><span lang=EN-US style='font-size:16.0pt;
line-height:107%'>Implementation</span></b></a></h1>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314829"><span lang=KO style='font-size:12.0pt;line-height:
107%'>제어 신호</span></a></h2>

<table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0 width=643
 style='width:482.0pt;margin-left:-14.45pt;border-collapse:collapse;border:
 none'>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=KO style='font-size:8.0pt;color:black'>신호 이름</span></b></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=KO style='font-size:8.0pt;color:black'>인가되지
  않은 경우 </span></b><b><span lang=EN-US style='font-size:8.0pt;color:black'>(0)</span></b></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=KO style='font-size:8.0pt;color:black'>인가된 경우
  </span></b><b><span lang=EN-US style='font-size:8.0pt;color:black'>(1)</span></b></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALUSrc</span></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALU</span><span lang=KO style='font-size:
  8.0pt'>의 두번째 피연산자로</span><span lang=EN-US style='font-size:8.0pt'> R[rt]</span><span
  lang=KO style='font-size:8.0pt'>가 들어옴</span></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALU</span><span lang=KO style='font-size:
  8.0pt'>의 두번째 피연산자로 부호 혹은 제로확장된</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>immediate</span><span lang=KO
  style='font-size:8.0pt'>가 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>RegWrite</span></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>아무 일도 생기지 않음</span></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>레지스터에 쓰기 활성화</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MemRead</span></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>아무 일도 생기지 않음</span></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Data memory</span><span lang=KO
  style='font-size:8.0pt'>에 읽기 활성화</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MemWrite</span></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>아무 일도 생기지 않음</span></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Data memory</span><span lang=KO
  style='font-size:8.0pt'>에 쓰기 활성화</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>SignZero</span></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALUSrc MUX</span><span lang=KO
  style='font-size:8.0pt'>로 부호확장된 </span><span lang=EN-US style='font-size:
  8.0pt'>immediate</span><span lang=KO style='font-size:8.0pt'>가 들어옴</span></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALUSrc MUX</span><span lang=KO
  style='font-size:8.0pt'>로 제로확장된 </span><span lang=EN-US style='font-size:
  8.0pt'>immediate</span><span lang=KO style='font-size:8.0pt'>가 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Shift</span></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALU </span><span lang=KO style='font-size:
  8.0pt'>첫번째 입력으로 </span><span lang=EN-US style='font-size:8.0pt'>R[rs]</span><span
  lang=KO style='font-size:8.0pt'>가 들어옴</span></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALU </span><span lang=KO style='font-size:
  8.0pt'>첫번째 입력으로 </span><span lang=EN-US style='font-size:8.0pt'>shamt</span><span
  lang=KO style='font-size:8.0pt'>가 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IFFlush</span></p>
  </td>
  <td width=293 valign=top style='width:219.7pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>아무 일도 생기지 않음</span></p>
  </td>
  <td width=284 valign=top style='width:212.65pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IF/ID </span><span lang=KO
  style='font-size:8.0pt'>파이프라인의 명령어를 </span><span lang=EN-US style='font-size:
  8.0pt'>0x00000000 </span><span lang=KO style='font-size:8.0pt'>으로 변경</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0 width=643
 style='width:482.0pt;margin-left:-14.45pt;border-collapse:collapse;border:
 none'>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=KO style='font-size:8.0pt;color:black'>신호 이름</span></b></p>
  </td>
  <td width=161 valign=top style='width:120.5pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=EN-US style='font-size:8.0pt;color:black'>0</span></b></p>
  </td>
  <td width=142 valign=top style='width:106.3pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=EN-US style='font-size:8.0pt;color:black'>1</span></b></p>
  </td>
  <td width=133 valign=top style='width:100.1pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=EN-US style='font-size:8.0pt;color:black'>2</span></b></p>
  </td>
  <td width=141 valign=top style='width:105.45pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><b><span lang=EN-US style='font-size:8.0pt;color:black'>3</span></b></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>RegDst</span></p>
  </td>
  <td width=161 valign=top style='width:120.5pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-top:0cm;margin-right:0cm;margin-bottom:0cm;
  margin-left:8.0pt;text-indent:-8.0pt;line-height:normal'><span lang=EN-US
  style='font-size:8.0pt'>ID/EX </span><span lang=KO style='font-size:8.0pt'>파이프라인의</span><span
  lang=EN-US style='font-size:8.0pt'> rt</span><span lang=KO style='font-size:
  8.0pt'>가</span></p>
  <p class=MsoNormal style='margin-top:0cm;margin-right:0cm;margin-bottom:0cm;
  margin-left:8.0pt;text-indent:-8.0pt;line-height:normal'><span lang=KO
  style='font-size:8.0pt'>레지스터의</span><span lang=EN-US style='font-size:8.0pt'>
  Write register</span></p>
  <p class=MsoNormal style='margin-top:0cm;margin-right:0cm;margin-bottom:0cm;
  margin-left:8.0pt;text-indent:-8.0pt;line-height:normal'><span lang=KO
  style='font-size:8.0pt'>번호 입력이 됨</span></p>
  </td>
  <td width=142 valign=top style='width:106.3pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ID/EX </span><span lang=KO
  style='font-size:8.0pt'>파이프라인의 </span><span lang=EN-US style='font-size:8.0pt'>rd</span><span
  lang=KO style='font-size:8.0pt'>가 레지스터의 </span><span lang=EN-US
  style='font-size:8.0pt'>Write register</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>번호 입력이 됨</span></p>
  </td>
  <td width=133 valign=top style='width:100.1pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>31</span><span lang=KO style='font-size:
  8.0pt'>이 레지스터의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Write register </span><span lang=KO
  style='font-size:8.0pt'>번호 입력이 됨</span></p>
  </td>
  <td width=141 valign=top style='width:105.45pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt'>없음</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MemtoReg</span></p>
  </td>
  <td width=161 valign=top style='width:120.5pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALU result</span><span lang=KO
  style='font-size:8.0pt'>가 </span><span lang=EN-US style='font-size:8.0pt'>Write
  data</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>입력이 됨</span></p>
  </td>
  <td width=142 valign=top style='width:106.3pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>데이터 메모리의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Read data</span><span lang=KO
  style='font-size:8.0pt'>가 레지스터의 </span><span lang=EN-US style='font-size:
  8.0pt'>Write data </span><span lang=KO style='font-size:8.0pt'>입력이 됨</span></p>
  </td>
  <td width=133 valign=top style='width:100.1pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MEM/WB </span><span lang=KO
  style='font-size:8.0pt'>파이프라인의 </span><span lang=EN-US style='font-size:8.0pt'>PC
  + 8</span><span lang=KO style='font-size:8.0pt'>이 레지스터의 </span><span
  lang=EN-US style='font-size:8.0pt'>Write data </span><span lang=KO
  style='font-size:8.0pt'>입력이 됨</span></p>
  </td>
  <td width=141 valign=top style='width:105.45pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MEM/WB </span><span lang=KO
  style='font-size:8.0pt'>파이프라인의 </span><span lang=EN-US style='font-size:8.0pt'>upperimm</span><span
  lang=KO style='font-size:8.0pt'>이 레지스터의 </span><span lang=EN-US
  style='font-size:8.0pt'>Write data </span><span lang=KO style='font-size:
  8.0pt'>입력이 됨</span></p>
  </td>
 </tr>
 <tr>
  <td width=66 valign=top style='width:49.65pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Jump</span></p>
  </td>
  <td width=161 valign=top style='width:120.5pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Predict MUX</span><span lang=KO
  style='font-size:8.0pt'>로 </span><span lang=EN-US style='font-size:8.0pt'>IDBr
  MUX</span><span lang=KO style='font-size:8.0pt'>의 출력이 들어옴</span></p>
  </td>
  <td width=142 valign=top style='width:106.3pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Predict MUX</span><span lang=KO
  style='font-size:8.0pt'>로 </span><span lang=EN-US style='font-size:8.0pt'>JumpAddr</span><span
  lang=KO style='font-size:8.0pt'>가 들어옴</span></p>
  </td>
  <td width=133 valign=top style='width:100.1pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Predict MUX</span><span lang=KO
  style='font-size:8.0pt'>로</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardA MUX</span><span lang=KO
  style='font-size:8.0pt'>의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>출력이 들어옴</span></p>
  </td>
  <td width=141 valign=top style='width:105.45pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt'>없음</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>추가적으로 표에 명시되지 않은 제어신호로는
</span><span lang=EN-US>BEQ, BNE, Branch, Equal, PCBranch, ALUOp</span><span
lang=KO>가 있다</span><span lang=EN-US>. BEQ</span><span lang=KO>와</span><span
lang=EN-US> BNE</span><span lang=KO>의 경우 명령어가 각각 </span><span lang=EN-US>beq,
bne</span><span lang=KO>이면 인가되고</span><span lang=EN-US>, Branch</span><span
lang=KO>는 이 둘을 </span><span lang=EN-US>OR</span><span lang=KO>연산한 신호이다</span><span
lang=EN-US>. </span><span lang=KO>식으로 표현하면 </span><span lang=EN-US>Branch = BEQ
| BNE</span><span lang=KO>와 같다</span><span lang=EN-US>. Equal</span><span
lang=KO>은 </span><span lang=EN-US>ID</span><span lang=KO>단계의 </span><span
lang=EN-US>comparator</span><span lang=KO>에서 </span><span lang=EN-US>IDForwardA
MUX</span><span lang=KO>와 </span><span lang=EN-US>IDForwardB MUX</span><span
lang=KO>의 출력 값이 같은 지 비교해서 같으면 인가되고 같지 않으면 인가되지 않는다</span><span lang=EN-US>.
PCBranch</span><span lang=KO>는 </span><span lang=EN-US>BNE</span><span lang=KO>와
</span><span lang=EN-US>NOT</span><span lang=KO>게이트를 통과한 </span><span
lang=EN-US>Equal</span><span lang=KO>을 </span><span lang=EN-US>AND</span><span
lang=KO>연산한 값과 </span><span lang=EN-US>BEQ</span><span lang=KO>와 </span><span
lang=EN-US>Equal</span><span lang=KO>을 </span><span lang=EN-US>AND</span><span
lang=KO>연산한 값 둘을 </span><span lang=EN-US>OR</span><span lang=KO>연산한 신호이다</span><span
lang=EN-US>. </span><span lang=KO>이 또한 식으로 표현하면 </span><span lang=EN-US>PCBranch
= (BNE &amp; !Equal) | (BEQ &amp; Equal) </span><span lang=KO>과 같다</span><span
lang=EN-US>. </span><span lang=KO>다시 말하면</span><span lang=EN-US>, Branch </span><span
lang=KO>신호는 명령어가 </span><span lang=EN-US>beq</span><span lang=KO>나 </span><span
lang=EN-US>bne</span><span lang=KO>이면 인가되고</span><span lang=EN-US>, PCBranch </span><span
lang=KO>신호는 분기 명령어가 </span><span lang=EN-US>taken</span><span lang=KO>되면 인가되는 신호라고
볼 수 있다</span><span lang=EN-US>. ALUOp</span><span lang=KO>의 경우 명령어가</span><span
lang=EN-US> R</span><span lang=KO>형식이면 </span><span lang=EN-US>funct</span><span
lang=KO>에 따라</span><span lang=EN-US>, </span><span lang=KO>이외 형식인 경우 </span><span
lang=EN-US>opcode</span><span lang=KO>에 따라서 </span><span lang=EN-US>ALU</span><span
lang=KO>에 어떤 연산을 할지 설정하는 신호이다</span><span lang=EN-US>.</span></p>

<h2><a name="_Toc104314830"><span lang=KO style='font-size:12.0pt;line-height:
107%'>파이프라인</span></a></h2>

<p class=MsoNormal align=center style='text-align:center'><span lang=EN-US><img
width=452 height=250 id="그림 11" src="README.fld/image008.jpg"></span><a
href="#_edn5" name="_ednref5" title=""><span class=MsoEndnoteReference><span
lang=EN-US><span class=MsoEndnoteReference><span lang=EN-US style='font-size:
10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[v]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>파이프라인은 각 단계 사이에 </span><span
lang=EN-US>latch</span><span lang=KO>를 말하는데</span><span lang=EN-US>, </span><span
lang=KO>클럭 신호가 상향 엣지일 때 </span><span lang=EN-US>latch</span><span lang=KO>에 저장된
데이터들을 다음 단계로 출력하고</span><span lang=EN-US>, </span><span lang=KO>하향 엣지일 때 이전 단계에서
출력된 데이터들을 저장한다</span><span lang=EN-US>. </span><span lang=KO>위 사진에서 </span><span
lang=EN-US>IF/ID, ID/EX, EX/MEM, MEM/WB</span><span lang=KO>가 파이프라인 </span><span
lang=EN-US>latch</span><span lang=KO>들이다</span><span lang=EN-US>. </span><span
lang=KO>이러한 파이프라인을 구현하기 위해서 필자가 작성한 프로그램에서는 구조체 배열을 사용하였다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=195 height=65 id="그림 12" src="README.fld/image009.jpg"></span></p>

<p class=MsoNormal><span lang=KO>위와 같이 구조체를 각 파이프라인마다 생성하고 파이프라인에 저장되는 데이터들을 구조체
멤버로 선언한다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=81 height=56 id="그림 17" src="README.fld/image010.jpg"
alt="텍스트, 실외이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal><span lang=KO>이처럼 구조체를 배열로 선언하고 한 </span><span lang=EN-US>Clock
Cycle</span><span lang=KO>이 끝날 때마다 아래 사진의 </span><span lang=EN-US>ifid[1] = ifid[0]</span><span
lang=KO>과 같이 </span><span lang=EN-US>[0]</span><span lang=KO>에 있는 데이터들을 </span><span
lang=EN-US>[1]</span><span lang=KO>로 넘겨준다면 매 </span><span lang=EN-US>Clock
Cycle</span><span lang=KO>마다 다음 단계로 데이터를 넘겨주는 파이프라인을 구현할 수 있다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=215 height=168 id="그림 18" src="README.fld/image011.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314831"><span lang=KO style='font-size:12.0pt;line-height:
107%'>전방전달 유닛</span></a></h2>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=451 height=279 id="그림 1" src="README.fld/image012.jpg"></span><a
href="#_edn6" name="_ednref6" title=""><span class=MsoEndnoteReference><span
lang=EN-US><span class=MsoEndnoteReference><span lang=EN-US style='font-size:
10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[vi]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>Data dependency</span><span
lang=KO>를 극복하기 위해 후속 파이프라인의 데이터를 </span><span lang=EN-US>ALU</span><span
lang=KO>의 입력으로 전달해주는 유닛이다</span><span lang=EN-US>. EX</span><span lang=KO>단계에서 </span><span
lang=EN-US>ALU</span><span lang=KO>에 입력되는 레지스터가 </span><span lang=EN-US>EX/MEM </span><span
lang=KO>파이프라인의 </span><span lang=EN-US>Write register</span><span lang=KO>와 동일하거나</span><span
lang=EN-US> MEM/WB </span><span lang=KO>파이프라인의 </span><span lang=EN-US>Write
register</span><span lang=KO>와 동일한 경우 전방전달 유닛에서 이를 감지하고 </span><span
lang=EN-US>MUX</span><span lang=KO>를 통해 해당 단계에서의 입력 값을 </span><span lang=EN-US>ALU</span><span
lang=KO>로 전달하게 된다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=282 height=108 id="그림 4" src="README.fld/image013.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal><span lang=EN-US>EX</span><span lang=KO>단계와</span><span
lang=EN-US> MEM</span><span lang=KO>단계 사이의 </span><span lang=EN-US>Data
dependency</span><span lang=KO>를 탐지하기 위한 조건문은 위와 같다</span><span lang=EN-US>. </span><span
lang=KO>필자가 작성한 프로그램에서 구현한 파이프라인은 </span><span lang=EN-US>lui </span><span
lang=KO>명령어를 지원하므로 </span><span lang=EN-US>MEM</span><span lang=KO>단계의 </span><span
lang=EN-US>upperimm</span><span lang=KO>을 가져오기 위해 </span><span lang=EN-US>EX/MEM
</span><span lang=KO>파이프라인의 </span><span lang=EN-US>MemtoReg </span><span
lang=KO>신호가 </span><span lang=EN-US>11(2)</span><span lang=KO>인 경우 전방전달 유닛에서 </span><span
lang=EN-US>EXMEMupperimm </span><span lang=KO>신호를 인가하는 기능을 추가하였다</span><span
lang=EN-US>. EX</span><span lang=KO>단계와 </span><span lang=EN-US>WB</span><span
lang=KO>단계 사이의 </span><span lang=EN-US>Data dependency</span><span lang=KO>를 탐지하기
위한 코드는 아래와 같다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=284 height=103 id="그림 7" src="README.fld/image014.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal><span lang=KO>전방전달 유닛이 후속 단계의 값을 </span><span lang=EN-US>ALU</span><span
lang=KO>로 전달하기 위해서는 앞에서 말했듯이 </span><span lang=EN-US>MUX</span><span lang=KO>에 신호를
인가해야 한다</span><span lang=EN-US>. </span><span lang=KO>이 중 </span><span
lang=EN-US>ALU</span><span lang=KO>의 첫번째 입력에 관여하는 신호는 </span><span lang=EN-US>ForwardA</span><span
lang=KO>와 </span><span lang=EN-US>EXMEMupperimmA, </span><span lang=KO>두번째 입력에 관여하는
신호는 </span><span lang=EN-US>ForwardB</span><span lang=KO>와 </span><span
lang=EN-US>EXMEMupperimmB</span><span lang=KO>이다</span><span lang=EN-US>. </span><span
lang=KO>이 신호들이 의미하는 바는 다음 표와 같다</span><span lang=EN-US>.</span></p>

<table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0 width=643
 style='width:482.0pt;margin-left:-14.45pt;border-collapse:collapse;border:
 none'>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>전방전달 신호</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>소스</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>설명</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardA = 00</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ID/EX </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ALU </span><span
  lang=KO style='font-size:8.0pt'>첫번째 입력으로 레지스터에서 읽은 값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardA = 10</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-top:0cm;margin-right:0cm;
  margin-bottom:0cm;margin-left:4.0pt;text-align:left;text-indent:-4.0pt;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ALU </span><span
  lang=KO style='font-size:8.0pt'>첫번째 입력으로 </span><span lang=EN-US
  style='font-size:8.0pt'>EX/MEM </span><span lang=KO style='font-size:8.0pt'>파이프라인의
  </span><span lang=EN-US style='font-size:8.0pt'>ALU result </span><span
  lang=KO style='font-size:8.0pt'>혹은</span></p>
  <p class=MsoNormal align=left style='margin-top:0cm;margin-right:0cm;
  margin-bottom:0cm;margin-left:4.0pt;text-align:left;text-indent:-4.0pt;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>upperimm</span><span
  lang=KO style='font-size:8.0pt'>이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardA = 01</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>WB</span><span
  lang=KO style='font-size:8.0pt'>단계</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ALU </span><span
  lang=KO style='font-size:8.0pt'>첫번째 입력으로 </span><span lang=EN-US
  style='font-size:8.0pt'>WB</span><span lang=KO style='font-size:8.0pt'>단계의 </span><span
  lang=EN-US style='font-size:8.0pt'>MemtoReg MUX</span><span lang=KO
  style='font-size:8.0pt'>의 출력값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardB = 00</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ID/EX </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ALU </span><span
  lang=KO style='font-size:8.0pt'>두번째 입력으로 레지스터에서 읽은 값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardB = 10</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ALU </span><span
  lang=KO style='font-size:8.0pt'>두번째 입력으로 </span><span lang=EN-US
  style='font-size:8.0pt'>EX/MEM </span><span lang=KO style='font-size:8.0pt'>파이프라인의
  </span><span lang=EN-US style='font-size:8.0pt'>ALU result </span><span
  lang=KO style='font-size:8.0pt'>혹은</span></p>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>upperimm</span><span
  lang=KO style='font-size:8.0pt'>이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardB = 01</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>WB</span><span
  lang=KO style='font-size:8.0pt'>단계</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ALU </span><span
  lang=KO style='font-size:8.0pt'>두번째 입력으로 </span><span lang=EN-US
  style='font-size:8.0pt'>WB</span><span lang=KO style='font-size:8.0pt'>단계의</span><span
  lang=EN-US style='font-size:8.0pt'> MemtoReg MUX</span><span lang=KO
  style='font-size:8.0pt'>의 출력값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EXMEMupperimmA =
  0</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardA MUX</span><span
  lang=KO style='font-size:8.0pt'>로 직전 명령어의 </span><span lang=EN-US
  style='font-size:8.0pt'>ALU result</span><span lang=KO style='font-size:8.0pt'>가
  들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EXMEMupperimmA =
  1</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardA MUX</span><span
  lang=KO style='font-size:8.0pt'>로 직전 명령어의 </span><span lang=EN-US
  style='font-size:8.0pt'>upperimm</span><span lang=KO style='font-size:8.0pt'>이
  들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EXMEMupperimmB =
  0</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardB MUX</span><span
  lang=KO style='font-size:8.0pt'>로 직전 명령어의 </span><span lang=EN-US
  style='font-size:8.0pt'>ALU result</span><span lang=KO style='font-size:8.0pt'>가
  들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=132 valign=top style='width:99.25pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EXMEMupperimmB =
  1</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=387 valign=top style='width:290.6pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=left style='margin-bottom:0cm;text-align:left;
  line-height:normal'><span lang=EN-US style='font-size:8.0pt'>ForwardB MUX</span><span
  lang=KO style='font-size:8.0pt'>로 직전 명령어의 </span><span lang=EN-US
  style='font-size:8.0pt'>upperimm</span><span lang=KO style='font-size:8.0pt'>이
  들어옴</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal align=left style='text-align:left;text-indent:10.0pt'><span
lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>이로써 </span><span
lang=EN-US>EX</span><span lang=KO>단계에서 발생하는 </span><span lang=EN-US>Data
dependency</span><span lang=KO>는 해결되었다</span><span lang=EN-US>. </span><span
lang=KO>하지만 필자가 작성한 프로그램에선 분기 연산을 </span><span lang=EN-US>EX</span><span
lang=KO>단계가 아닌 </span><span lang=EN-US>ID</span><span lang=KO>단계에서 수행하도록 데이터패스를
수정하였다</span><span lang=EN-US>. </span><span lang=KO>이때</span><span lang=EN-US>,
ID</span><span lang=KO>단계에 </span><span lang=EN-US>bne $1, $0, (target) </span><span
lang=KO>명령어가 수행 중이고 </span><span lang=EN-US>EX</span><span lang=KO>단계에 </span><span
lang=EN-US>add $1 &lt;- $2, $3</span><span lang=KO>가 수행 중이라 가정해보자</span><span
lang=EN-US>. EX</span><span lang=KO>단계의 명령어가 </span><span lang=EN-US>WB</span><span
lang=KO>단계까지 진행되어 </span><span lang=EN-US>1</span><span lang=KO>번 레지스터에 값이 작성되도록
</span><span lang=EN-US>2cycle</span><span lang=KO>동안 </span><span lang=EN-US>NOP</span><span
lang=KO>를 추가하거나</span><span lang=EN-US>, 1cycle</span><span lang=KO>만 </span><span
lang=EN-US>NOP</span><span lang=KO>를 추가하고 명령어가 </span><span lang=EN-US>MEM</span><span
lang=KO>단계로 진행되었을 때 전방전달을 하는 방법이 있다</span><span lang=EN-US>. </span><span
lang=KO>필자는 후자를 통해 새롭게 발생한 </span><span lang=EN-US>dependency</span><span
lang=KO>를 해결하였다</span><span lang=EN-US>. </span><span lang=KO>아래 사진과 같은 전방전달 유닛을
</span><span lang=EN-US>ID</span><span lang=KO>단계에 추가한다면 </span><span
lang=EN-US>1cycle</span><span lang=KO>의</span><span lang=EN-US> NOP</span><span
lang=KO>만으로 이 문제를 해결할 수 있다</span><span lang=EN-US>. </span><span lang=KO>단</span><span
lang=EN-US>, beq</span><span lang=KO>나 </span><span lang=EN-US>bne </span><span
lang=KO>직전 명령어가 </span><span lang=EN-US>lw</span><span lang=KO>라면 </span><span
lang=EN-US>MEM</span><span lang=KO>단계가 모두 진행되어야 값을 알 수 있기 때문에 </span><span
lang=EN-US>2cycle</span><span lang=KO>의 </span><span lang=EN-US>NOP</span><span
lang=KO>가 필요하다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=280 height=319 id="그림 26" src="README.fld/image015.jpg"></span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>ID </span><span
lang=KO>전방전달 유닛은 </span><span lang=EN-US>EX</span><span lang=KO>단계에서의 것과 크게 다르지
않다</span><span lang=EN-US>. </span><span lang=KO>다만</span><span lang=EN-US>,
ID/EX </span><span lang=KO>파이프라인에서 전방전달 해야 할 데이터는 </span><span lang=EN-US>upperimm</span><span
lang=KO>밖에 없다는 것이 특징이다</span><span lang=EN-US>. ALU </span><span lang=KO>명령어가 직전으로
들어온다면 반드시 </span><span lang=EN-US>NOP</span><span lang=KO>를 거쳐야 하기 때문이다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=394 height=70 id="그림 14" src="README.fld/image016.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>ID </span><span lang=KO>전방전달 유닛에서 인가하는 신호들이
의미하는 바는 다음 표와 같다</span><span lang=EN-US>.</span></p>

<table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0 width=643
 style='width:482.0pt;margin-left:-14.45pt;border-collapse:collapse;border:
 none'>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>전방전달 신호</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>소스</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>설명</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardA = 00</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>레지스터</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 첫번째 입력으로 레지스터에서 읽은 값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardA = 10</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 첫번째 입력으로 </span><span lang=EN-US style='font-size:
  8.0pt'>EX/MEM </span><span lang=KO style='font-size:8.0pt'>파이프라인의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALU result </span><span lang=KO
  style='font-size:8.0pt'>혹은 </span><span lang=EN-US style='font-size:8.0pt'>upperimm</span><span
  lang=KO style='font-size:8.0pt'>이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardA = 01</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>WB</span><span lang=KO style='font-size:
  8.0pt'>단계</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 첫번째 입력으로 </span><span lang=EN-US style='font-size:
  8.0pt'>WB</span><span lang=KO style='font-size:8.0pt'>단계의</span><span
  lang=EN-US style='font-size:8.0pt'> MemtoReg MUX</span><span lang=KO
  style='font-size:8.0pt'>의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>출력값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardA = 11</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><a
  name="OLE_LINK1"><span lang=EN-US style='font-size:8.0pt'>ID/EX </span></a><span
  lang=KO style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 첫번째 입력으로 </span><span lang=EN-US style='font-size:
  8.0pt'>ID/EX </span><span lang=KO style='font-size:8.0pt'>파이프라인의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>upperimm</span><span lang=KO
  style='font-size:8.0pt'>이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardB = 00</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>레지스터</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 두번째 입력으로 레지스터에서 읽은 값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardB = 10</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 두번째 입력으로 </span><span lang=EN-US style='font-size:
  8.0pt'>EX/MEM </span><span lang=KO style='font-size:8.0pt'>파이프라인의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ALU result </span><span lang=KO
  style='font-size:8.0pt'>혹은 </span><span lang=EN-US style='font-size:8.0pt'>upperimm</span><span
  lang=KO style='font-size:8.0pt'>이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardB = 01</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>WB</span><span lang=KO style='font-size:
  8.0pt'>단계</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 두번째 입력으로 </span><span lang=EN-US style='font-size:
  8.0pt'>WB</span><span lang=KO style='font-size:8.0pt'>단계의</span><span
  lang=EN-US style='font-size:8.0pt'> MemtoReg MUX</span><span lang=KO
  style='font-size:8.0pt'>의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>출력값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardB = 11</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ID/EX </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>Comparator</span><span lang=KO
  style='font-size:8.0pt'>의 두번째 입력으로 </span><span lang=EN-US style='font-size:
  8.0pt'>ID/EX </span><span lang=KO style='font-size:8.0pt'>파이프라인의</span></p>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>upperimm</span><span lang=KO
  style='font-size:8.0pt'>이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ID_EXMEMupperimmA = 0</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardA MUX</span><span lang=KO
  style='font-size:8.0pt'>로 </span><span lang=EN-US style='font-size:8.0pt'>EX/MEM
  </span><span lang=KO style='font-size:8.0pt'>파이프라인의 </span><span lang=EN-US
  style='font-size:8.0pt'>ALU result</span><span lang=KO style='font-size:8.0pt'>가
  들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ID_EXMEMupperimmA = 1</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardA MUX</span><span lang=KO
  style='font-size:8.0pt'>로 </span><span lang=EN-US style='font-size:8.0pt'>EX/MEM
  </span><span lang=KO style='font-size:8.0pt'>파이프라인의 </span><span lang=EN-US
  style='font-size:8.0pt'>upperimm</span><span lang=KO style='font-size:8.0pt'>이
  들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ID_EXMEMupperimmB = 0</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardB MUX</span><span lang=KO
  style='font-size:8.0pt'>로 </span><span lang=EN-US style='font-size:8.0pt'>EX/MEM
  </span><span lang=KO style='font-size:8.0pt'>파이프라인의 </span><span lang=EN-US
  style='font-size:8.0pt'>ALU result</span><span lang=KO style='font-size:8.0pt'>가
  들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=151 valign=top style='width:113.45pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>ID_EXMEMupperimmB = 1</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=369 valign=top style='width:276.4pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>IDForwardB MUX</span><span lang=KO
  style='font-size:8.0pt'>로 </span><span lang=EN-US style='font-size:8.0pt'>EX/MEM
  </span><span lang=KO style='font-size:8.0pt'>파이프라인의 </span><span lang=EN-US
  style='font-size:8.0pt'>upperimm</span><span lang=KO style='font-size:8.0pt'>이
  들어옴</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>MEM</span><span
lang=KO>단계에서도 </span><span lang=EN-US>Data dependency</span><span lang=KO>가 일어날
수 있다</span><span lang=EN-US>. </span><span lang=KO>예를 들어 </span><span
lang=EN-US>lw $1 &lt;- 0($sp) </span><span lang=KO>다음 명령어가  </span><span
lang=EN-US>sw $1 -&gt; 4($sp)</span><span lang=KO>라고 생각해보자</span><span
lang=EN-US>. MEM</span><span lang=KO>단계에 </span><span lang=EN-US>sw $1 -&gt;
4($sp)</span><span lang=KO>가 존재하고 </span><span lang=EN-US>WB</span><span
lang=KO>단계에 </span><span lang=EN-US>lw $1 &lt;- 0($sp)</span><span lang=KO>가 존재할
때</span><span lang=EN-US>, sw</span><span lang=KO>는 </span><span lang=EN-US>lw</span><span
lang=KO>가 완전히 끝나도록 </span><span lang=EN-US>1cycle</span><span lang=KO>의 </span><span
lang=EN-US>Stalling</span><span lang=KO>을 거쳐야 올바른 값을 메모리에 작성할 수 있다</span><span
lang=EN-US>. </span><span lang=KO>메모리의 특정 주소의 값을 다른 주소에 복사할 때마다 </span><span
lang=EN-US>Stalling</span><span lang=KO>이 추가된다면</span><span lang=EN-US>, </span><span
lang=KO>매우 비효율적일 것이다</span><span lang=EN-US>. </span><span lang=KO>따라서 </span><span
lang=EN-US>MEM</span><span lang=KO>단계에서도 전방전달 유닛이 필요하다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=118 height=225 id="그림 25" src="README.fld/image017.jpg"></span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>MEM </span><span
lang=KO>전방전달 유닛이 제대로 작동하기 위해선 </span><span lang=EN-US>MEM/WB </span><span
lang=KO>파이프라인에 </span><span lang=EN-US>EX/MEM </span><span lang=KO>파이프라인의</span><span
lang=EN-US>MemWrite </span><span lang=KO>신호가 저장될 필요가 있다</span><span lang=EN-US>.
MEM/WB </span><span lang=KO>파이프라인에</span><span lang=EN-US> MemWrite </span><span
lang=KO>신호와 </span><span lang=EN-US>EX/MEM </span><span lang=KO>파이프라인에 </span><span
lang=EN-US>MemRead</span><span lang=KO>가 신호가 인가되어 있고</span><span lang=EN-US>,
WB</span><span lang=KO>단계에서 작성할 레지스터 번호와 </span><span lang=EN-US>MEM</span><span
lang=KO>단계에서 메모리에 저장할 레지스터 번호가 같다면 </span><span lang=EN-US>WB</span><span
lang=KO>단계의 </span><span lang=EN-US>MemtoReg MUX </span><span lang=KO>출력을 </span><span
lang=EN-US>MEM</span><span lang=KO>단계로 전방전달하는 방식이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='margin-left:5.0pt;text-align:center;
text-indent:5.0pt'><span lang=EN-US><img width=394 height=35 id="그림 40"
src="README.fld/image018.jpg"></span></p>

<p class=MsoNormal><span lang=EN-US>MEM </span><span lang=KO>전방전달 유닛에서 인가하는 신호가
의미하는 바는 다음 표와 같다</span><span lang=EN-US>.</span></p>

<table class=MsoTableGrid border=1 cellspacing=0 cellpadding=0 width=633
 style='width:474.9pt;margin-left:-7.35pt;border-collapse:collapse;border:none'>
 <tr>
  <td width=104 valign=top style='width:78.0pt;border:solid windowtext 1.0pt;
  background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>전방전달 신호</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>소스</span></p>
  </td>
  <td width=406 valign=top style='width:304.75pt;border:solid windowtext 1.0pt;
  border-left:none;background:#B4C6E7;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal align=center style='margin-bottom:0cm;text-align:center;
  line-height:normal'><span lang=KO style='font-size:8.0pt;color:black'>설명</span></p>
  </td>
 </tr>
 <tr>
  <td width=104 valign=top style='width:78.0pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MEMForward = 0</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>EX/MEM </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=406 valign=top style='width:304.75pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>데이터 메모리의 </span><span lang=EN-US
  style='font-size:8.0pt'>Write data</span><span lang=KO style='font-size:8.0pt'>로
  </span><span lang=EN-US style='font-size:8.0pt'>ForwardB MUX</span><span
  lang=KO style='font-size:8.0pt'>의 출력값이 들어옴</span></p>
  </td>
 </tr>
 <tr>
  <td width=104 valign=top style='width:78.0pt;border:solid windowtext 1.0pt;
  border-top:none;padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MEMForward = 1</span></p>
  </td>
  <td width=123 valign=top style='width:92.15pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=EN-US style='font-size:8.0pt'>MEM/WB </span><span lang=KO
  style='font-size:8.0pt'>파이프라인</span></p>
  </td>
  <td width=406 valign=top style='width:304.75pt;border-top:none;border-left:
  none;border-bottom:solid windowtext 1.0pt;border-right:solid windowtext 1.0pt;
  padding:0cm 5.4pt 0cm 5.4pt'>
  <p class=MsoNormal style='margin-bottom:0cm;line-height:normal'><span
  lang=KO style='font-size:8.0pt'>데이터 메모리의 </span><span lang=EN-US
  style='font-size:8.0pt'>Write data</span><span lang=KO style='font-size:8.0pt'>로
  </span><span lang=EN-US style='font-size:8.0pt'>WB</span><span lang=KO
  style='font-size:8.0pt'>단계의 </span><span lang=EN-US style='font-size:8.0pt'>MemtoReg
  MUX</span><span lang=KO style='font-size:8.0pt'>의 출력값이 들어옴</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal style='margin-left:5.0pt;text-indent:5.0pt'><span
lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314832"><span lang=KO style='font-size:12.0pt;line-height:
107%'>해저드 검출 유닛</span></a></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>전방전달 유닛들이 모든 </span><span
lang=EN-US>Data dependency</span><span lang=KO>를 해결하는 것은 아니다</span><span
lang=EN-US>. </span><span lang=KO>다음 사진과 같은 경우를 생각해보자</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=347 height=235 id="그림 24" src="README.fld/image019.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><a
href="#_edn7" name="_ednref7" title=""><span class=MsoEndnoteReference><span
lang=EN-US><span class=MsoEndnoteReference><span lang=EN-US style='font-size:
10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[vii]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US
style='line-height:107%'>lw</span><span lang=KO style='line-height:107%'>에서 작성하는
레지스터와 그 다음 명령어인 </span><span lang=EN-US style='line-height:107%'>and</span><span
lang=KO style='line-height:107%'>에서 읽는 레지스터가 </span><span lang=EN-US
style='line-height:107%'>$2</span><span lang=KO style='line-height:107%'>로 같다</span><span
lang=EN-US style='line-height:107%'>. lw</span><span lang=KO style='line-height:
107%'>에서 </span><span lang=EN-US style='line-height:107%'>$2</span><span
lang=KO style='line-height:107%'>에 쓰이는 값은 </span><span lang=EN-US
style='line-height:107%'>MEM</span><span lang=KO style='line-height:107%'>단계가 완료되어야
확정되므로 한 단계의 </span><span lang=EN-US style='line-height:107%'>Stalling</span><span
lang=KO style='line-height:107%'>이 필요하다</span><span lang=EN-US
style='line-height:107%'>. </span><span lang=KO style='line-height:107%'>그렇지 않으면
</span><span lang=EN-US style='line-height:107%'>Clock Cycle 5</span><span
lang=KO style='line-height:107%'>의 </span><span lang=EN-US style='line-height:
107%'>WB</span><span lang=KO style='line-height:107%'>단계의 값을 </span><span
lang=EN-US style='line-height:107%'>Clock Cycle 4</span><span lang=KO
style='line-height:107%'>의 </span><span lang=EN-US style='line-height:107%'>EX</span><span
lang=KO style='line-height:107%'>단계로 전방전달해야 하는 시간적 모순이 발생하게 된다</span><span
lang=EN-US style='line-height:107%'>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US style='line-height:107%'><img width=338 height=219 id="그림 27"
src="README.fld/image020.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><a
href="#_edn8" name="_ednref8" title=""><span class=MsoEndnoteReference><span
lang=EN-US style='line-height:107%'><span class=MsoEndnoteReference><span
lang=EN-US style='font-size:10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[viii]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal><span lang=EN-US style='line-height:107%'>and </span><span
lang=KO style='line-height:107%'>명령어를 </span><span lang=EN-US style='line-height:
107%'>NOP</span><span lang=KO style='line-height:107%'>로 바꾸어 한 </span><span
lang=EN-US style='line-height:107%'>cycle stalling</span><span lang=KO
style='line-height:107%'>을 하게 되면</span><span lang=EN-US style='line-height:
107%'>, Clock Cycle 5</span><span lang=KO style='line-height:107%'>의</span><span
lang=EN-US style='line-height:107%'> WB</span><span lang=KO style='line-height:
107%'>단계의 값을 같은 </span><span lang=EN-US style='line-height:107%'>Clock Cycle 5</span><span
lang=KO style='line-height:107%'>의 </span><span lang=EN-US style='line-height:
107%'>EX</span><span lang=KO style='line-height:107%'>단계로 전달하게 되므로 시간적 모순이 해결되어
정상적으로 전방전달이 가능하게 된다</span><span lang=EN-US style='line-height:107%'>. </span><span
lang=KO style='line-height:107%'>이러한 해저드를 </span><span lang=EN-US
style='line-height:107%'>Load-use hazard</span><span lang=KO style='line-height:
107%'>라고 한다</span><span lang=EN-US style='line-height:107%'>. </span><span
lang=KO style='line-height:107%'>이 해저드를 검출하기 위해선 또다른 유닛이 필요하다</span><span
lang=EN-US style='line-height:107%'>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US style='line-height:107%'><img width=150 height=172 id="그림 28"
src="README.fld/image021.jpg"></span></p>

<p class=MsoNormal><span lang=KO style='line-height:107%'>해저드 검출 유닛은 </span><span
lang=EN-US style='line-height:107%'>ID</span><span lang=KO style='line-height:
107%'>단계에서 </span><span lang=EN-US style='line-height:107%'>rs</span><span
lang=KO style='line-height:107%'>나</span><span lang=EN-US style='line-height:
107%'> rt</span><span lang=KO style='line-height:107%'>값이 </span><span
lang=EN-US style='line-height:107%'>EX</span><span lang=KO style='line-height:
107%'>단계의 </span><span lang=EN-US style='line-height:107%'>rt</span><span
lang=KO style='line-height:107%'>값과 같을 경우 </span><span lang=EN-US
style='line-height:107%'>PC</span><span lang=KO style='line-height:107%'>와 </span><span
lang=EN-US style='line-height:107%'>IF/ID </span><span lang=KO
style='line-height:107%'>파이프라인</span><span lang=EN-US style='line-height:107%'>,
BTB(Branch Target Buffer), ID/EX </span><span lang=KO style='line-height:107%'>파이프라인의
제어 신호들을 </span><span lang=EN-US style='line-height:107%'>0</span><span lang=KO
style='line-height:107%'>으로 만듦으로써 아무것도 하지 않는 </span><span lang=EN-US
style='line-height:107%'>NOP</span><span lang=KO style='line-height:107%'>를 만들어낸다</span><span
lang=EN-US style='line-height:107%'>. Load-use hazard</span><span lang=KO
style='line-height:107%'>를 감지하고 </span><span lang=EN-US style='line-height:
107%'>Stalling</span><span lang=KO style='line-height:107%'>하기 위한 코드는 다음과 같다</span><span
lang=EN-US style='line-height:107%'>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US style='line-height:107%'><img width=266 height=57 id="그림 29"
src="README.fld/image022.jpg" alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO style='line-height:
107%'>하지만</span><span lang=EN-US style='line-height:107%'>, ID</span><span
lang=KO style='line-height:107%'>단계에서 분기 연산을 하는 데이터패스에서 만약 </span><span
lang=EN-US style='line-height:107%'>lw</span><span lang=KO style='line-height:
107%'>명령어 바로 뒤에 </span><span lang=EN-US style='line-height:107%'>beq </span><span
lang=KO style='line-height:107%'>혹은 </span><span lang=EN-US style='line-height:
107%'>bne</span><span lang=KO style='line-height:107%'>가 뒤따라 온다면 </span><span
lang=EN-US style='line-height:107%'>1cycle</span><span lang=KO
style='line-height:107%'>의 </span><span lang=EN-US style='line-height:107%'>Stalling</span><span
lang=KO style='line-height:107%'>으로는 올바른 결과를 도출해낼 수 없다</span><span lang=EN-US
style='line-height:107%'>. </span><span lang=KO style='line-height:107%'>앞에서 말했듯이
</span><span lang=EN-US style='line-height:107%'>lw</span><span lang=KO
style='line-height:107%'>명령어의 결과는 </span><span lang=EN-US style='line-height:
107%'>MEM</span><span lang=KO style='line-height:107%'>단계가 완료되어야 확정되므로 </span><span
lang=EN-US style='line-height:107%'>beq </span><span lang=KO style='line-height:
107%'>혹은 </span><span lang=EN-US style='line-height:107%'>bne</span><span
lang=KO style='line-height:107%'>와 </span><span lang=EN-US style='line-height:
107%'>lw</span><span lang=KO style='line-height:107%'>명령어 사이의 거리가 최소 </span><span
lang=EN-US style='line-height:107%'>3</span><span lang=KO style='line-height:
107%'>이 되도록 해야 한다</span><span lang=EN-US style='line-height:107%'>. </span><span
lang=KO style='line-height:107%'>따라서 이 경우 </span><span lang=EN-US
style='line-height:107%'>2cycle</span><span lang=KO style='line-height:107%'>의 </span><span
lang=EN-US style='line-height:107%'>Stalling</span><span lang=KO
style='line-height:107%'>이 필요하다</span><span lang=EN-US style='line-height:107%'>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US style='line-height:107%'><img width=354 height=140 id="그림 31"
src="README.fld/image023.jpg" alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal><span lang=KO style='line-height:107%'>위와 같이 조건을 추가한다면 분기 연산과
</span><span lang=EN-US style='line-height:107%'>lw</span><span lang=KO
style='line-height:107%'>간에 </span><span lang=EN-US style='line-height:107%'>2cycle</span><span
lang=KO style='line-height:107%'>의 </span><span lang=EN-US style='line-height:
107%'>NOP</span><span lang=KO style='line-height:107%'>를 추가함으로써 해결할 수 있다</span><span
lang=EN-US style='line-height:107%'>.</span></p>

<p class=MsoNormal><span lang=EN-US style='line-height:107%'>&nbsp;</span></p>

<h2><a name="_Toc104314833"><span lang=KO style='font-size:12.0pt;line-height:
107%'>분기 예측 </span></a><span lang=EN-US style='font-size:12.0pt;line-height:
107%'>Scheme</span></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>앞선 </span><span
lang=EN-US>Introduction-</span><span lang=KO>분기 예측 항목에서 발언한 것과 같이 성능 손실을 최대한 줄이기
위하여 고안된 것이 분기 예측이라고 할 수 있다</span><span lang=EN-US>. Always not-taken</span><span
lang=KO>을 제외한 </span><span lang=EN-US>scheme</span><span lang=KO>은 </span><span
lang=EN-US>Branch Target Buffer</span><span lang=KO>가 필요한데</span><span
lang=EN-US>, </span><span lang=KO>이는 </span><span lang=EN-US>IF</span><span
lang=KO>단계와 </span><span lang=EN-US>ID</span><span lang=KO>단계에 걸쳐서 사용된다</span><span
lang=EN-US>. </span><span lang=KO>먼저 </span><span lang=EN-US>IF</span><span
lang=KO>단계에서 </span><span lang=EN-US>PC</span><span lang=KO>값이 버퍼에 저장된 분기 명령어의 </span><span
lang=EN-US>PC</span><span lang=KO>값과 일치하는지 판단하고</span><span lang=EN-US>, ID</span><span
lang=KO>단계에서 예측이 </span><span lang=EN-US>HIT</span><span lang=KO>했는지 확인 후 결과에 따라
예측 비트를 업데이트 하거나</span><span lang=EN-US>, </span><span lang=KO>만약 </span><span
lang=EN-US>PC</span><span lang=KO>값이 버퍼에 없었다면 버퍼에 </span><span lang=EN-US>PC</span><span
lang=KO>값과 분기 목적지 주소를 저장한다</span><span lang=EN-US>. </span><span lang=KO>또한</span><span
lang=EN-US>, </span><span lang=KO>동적 분기 예측인 경우 </span><span lang=EN-US>Direction
predictor</span><span lang=KO>필자가 작성한 프로그램에서 구현한 분기 예측 </span><span lang=EN-US>scheme</span><span
lang=KO>은 정적 분기 예측 중 </span><span lang=EN-US>Always not-taken, Always taken,
Backward Taken-Forward Not Taken</span><span lang=KO>과 동적 분기 예측 중 </span><span
lang=EN-US>One-Level branch predictor(1bit, 2bit-Saturating, 2bit-Hysteresis),
Two-Level Gshare branch predictor(1bit, 2bit-Saturating, 2bit-Hysteresis)</span><span
lang=KO>이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>&nbsp;</span></p>

<h3 style='margin-left:50.0pt;text-indent:-20.0pt'><a name="_Toc104314834"><span
lang=EN-US>Always not-taken</span></a></h3>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>항상 분기하지 않는다고 예측하는 것이다</span><span
lang=EN-US>. Unconditional jump </span><span lang=KO>명령어가 아닌 한 어떤 명령어가 들어와도 </span><span
lang=EN-US>PC + 4</span><span lang=KO>를 다음 </span><span lang=EN-US>PC</span><span
lang=KO>값으로 설정하고</span><span lang=EN-US>, ID</span><span lang=KO>단계에서 분기가 아님으로 판정되면
그대로 진행하고</span><span lang=EN-US>, </span><span lang=KO>분기로 판정되면 </span><span
lang=EN-US>IF/ID </span><span lang=KO>파이프라인의 명령어를 </span><span lang=EN-US>0</span><span
lang=KO>으로 만들고 계산된 분기 주소로 점프한다</span><span lang=EN-US>. </span><span lang=KO>이 방식은
따로 </span><span lang=EN-US>BTB</span><span lang=KO>와 같은 분기 예측기를 둘 필요가 없으므로 버퍼에서
확인하는 과정과 업데이트하는 과정이 없어서 구조가 간단하다는 것이 장점이다</span><span lang=EN-US>. </span><span
lang=KO>하지만 프로그램이 분기를 타는 과정이 많은 경우</span><span lang=EN-US>, </span><span
lang=KO>적중률이 낮아진다는 것이 단점이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>&nbsp;</span></p>

<h3 style='margin-left:50.0pt;text-indent:-20.0pt'><a name="_Toc104314835"><span
lang=EN-US>Always taken</span></a></h3>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>이 </span><span
lang=EN-US>scheme</span><span lang=KO>부터 분기 예측기인 </span><span lang=EN-US>BTB</span><span
lang=KO>가 필요하다</span><span lang=EN-US>. PC</span><span lang=KO>값이 이전에 </span><span
lang=EN-US>beq</span><span lang=KO>나 </span><span lang=EN-US>bne</span><span
lang=KO>명령어를 가리켰는지 확인하는 과정이 필요하기 때문이다</span><span lang=EN-US>. </span><span
lang=KO>분기 명령어를 한번 처리해서 </span><span lang=EN-US>BTB</span><span lang=KO>에</span><span
lang=EN-US> PC</span><span lang=KO>값이 저장되면</span><span lang=EN-US>, </span><span
lang=KO>그 다음부터 </span><span lang=EN-US>IF</span><span lang=KO>단계에서 확인 후 항상 분기를 탄다고
예측하여 분기 목적지 주소로 </span><span lang=EN-US>PC</span><span lang=KO>를 업데이트 한다</span><span
lang=EN-US>. </span><span lang=KO>아래 사진은 </span><span lang=EN-US>IF</span><span
lang=KO>단계에서 </span><span lang=EN-US>BTB</span><span lang=KO>를 확인하는 코드이다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=287 height=198 id="그림 37" src="README.fld/image024.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US>&nbsp;</span></p>

<h3 style='margin-left:50.0pt;text-indent:-20.0pt'><a name="_Toc104314836"><span
lang=EN-US>Backward Taken, Forward Not Taken(BTFNT)</span></a></h3>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>BTB</span><span
lang=KO>에 저장된 분기 목적지 주소가 </span><span lang=EN-US>PC</span><span lang=KO>값보다 작으면
분기를 탄다고 예측하고</span><span lang=EN-US>, </span><span lang=KO>아니면 분기를 안탄다고 예측하는 </span><span
lang=EN-US>scheme</span><span lang=KO>이다</span><span lang=EN-US>. </span><span
lang=KO>이는 분기 명령어가 </span><span lang=EN-US>for, while</span><span lang=KO>처럼 반복문을
의미하는 경우 매우 높은 적중률을 보인다</span><span lang=EN-US>. </span><span lang=KO>아래 사진은 </span><span
lang=EN-US>BTB</span><span lang=KO>에서 분기 목적지 주소가 </span><span lang=EN-US>PC</span><span
lang=KO>값보다 큰지 혹은 작은지를 확인하는 과정을 구현한 것이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=290 height=108 id="그림 39" src="README.fld/image025.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US>&nbsp;</span></p>

<h3 style='margin-left:50.0pt;text-indent:-20.0pt'><a name="_Toc104314837"><span
lang=EN-US>One-Level branch predictor</span></a></h3>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>이 </span><span
lang=EN-US>scheme</span><span lang=KO>부터 과거의 분기 이력을 참조하여 동적으로 분기를 예측하게 된다</span><span
lang=EN-US>. </span><span lang=KO>과거 이력을 참조할 때 카운터라는 </span><span lang=EN-US>State
machine</span><span lang=KO>이 쓰이게 된다</span><span lang=EN-US>. 1</span><span
lang=KO>비트 카운터는 단순히 직전의 분기 이력을 저장하는 장치에 불과하다</span><span lang=EN-US>. </span><span
lang=KO>직전 분기 명령어가 분기를 탔다면 </span><span lang=EN-US>1, </span><span lang=KO>아니면 </span><span
lang=EN-US>0</span><span lang=KO>인 상태이다</span><span lang=EN-US>. </span><span
lang=KO>이는 분기가 거의 항상 일어날지라도</span><span lang=EN-US>, </span><span lang=KO>분기가 일어나지
않을 때는 한 번이 아닌 두 번의 잘못된 예측을 할 가능성이 높게 되는 문제점을 갖고 있다</span><span lang=EN-US>. </span><span
lang=KO>이 같은 약점을 보완하기 위해 </span><span lang=EN-US>2</span><span lang=KO>비트 카운터를 사용한다</span><span
lang=EN-US>.<a href="#_edn9" name="_ednref9" title=""><span
class=MsoEndnoteReference><span class=MsoEndnoteReference><span lang=EN-US
style='font-size:10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[ix]</span></span></span></a></span><span
lang=EN-US style='font-size:8.0pt;line-height:107%'> (David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
style='position:relative;z-index:251663360'><span style='left:0px;position:
absolute;left:281px;top:-3454px;width:23px;height:21px'><img width=23
height=21 src=missing.gif></span></span><span style='position:relative;
z-index:251667456'><span style='left:0px;position:absolute;left:139px;
top:-3507px;width:24px;height:21px'><img width=24 height=21 src=missing.gif></span></span><span
style='position:relative;z-index:251665408'><span style='left:0px;position:
absolute;left:281px;top:-3507px;width:23px;height:21px'><img width=23
height=21 src=missing.gif></span></span><span style='position:relative;
z-index:251661312'><span style='left:0px;position:absolute;left:139px;
top:-3454px;width:24px;height:21px'><img width=24 height=21 src=missing.gif></span></span><span
lang=EN-US><img width=235 height=142 id="그림 44" src="README.fld/image026.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><a
href="#_edn10" name="_ednref10" title=""><span class=MsoEndnoteReference><span
lang=EN-US><span class=MsoEndnoteReference><span lang=EN-US style='font-size:
10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[x]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(David A. Patterson, John
L. Hennessy, n.d.)</span></p>

<p class=MsoNormal><span lang=KO>위 사진은 </span><span lang=EN-US>2</span><span
lang=KO>비트</span><span lang=EN-US> Saturating </span><span lang=KO>카운터이다</span><span
lang=EN-US>. </span><span lang=KO>간단히 말해 </span><span lang=EN-US>00</span><span
lang=KO>상태에서 </span><span lang=EN-US>2</span><span lang=KO>번 연속으로 분기를 탔다면 다음부터 그
분기는 탄다고 예측하고</span><span lang=EN-US>, 11</span><span lang=KO>상태에서 </span><span
lang=EN-US>2</span><span lang=KO>번 연속으로 분기를 타지 않았다면 다음부터 그 분기는 타지 않는다고 예측한다</span><span
lang=EN-US>. 2</span><span lang=KO>비트</span><span lang=EN-US> Hysteresis </span><span
lang=KO>카운터는 </span><span lang=EN-US>00</span><span lang=KO>상태에서 </span><span
lang=EN-US>2</span><span lang=KO>번 연속으로 분기를 탔다면 </span><span lang=EN-US>11</span><span
lang=KO>로 바로 상태를 변경하고</span><span lang=EN-US>, 11</span><span lang=KO>상태에서 </span><span
lang=EN-US>2</span><span lang=KO>번 연속으로 분기를 타지 않았다면 </span><span lang=EN-US>00</span><span
lang=KO>으로 바로 상태를 변경한다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center'><span lang=EN-US><img
width=288 height=156 id="그림 50" src="README.fld/image027.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center'><a href="#_edn11"
name="_ednref11" title=""><span class=MsoEndnoteReference><span lang=EN-US><span
class=MsoEndnoteReference><span lang=EN-US style='font-size:10.0pt;line-height:
107%;font-family:"맑은 고딕",sans-serif'>[xi]</span></span></span></span></a><span
lang=EN-US> </span><span lang=EN-US style='font-size:8.0pt;line-height:107%'>(Carnegie
Mellon University, 2015)</span></p>

<p class=MsoNormal><span lang=KO style='line-height:107%'>위 사진이 </span><span
lang=EN-US style='line-height:107%'>One-Level branch predictor</span><span
lang=KO style='line-height:107%'>의 구조이다</span><span lang=EN-US
style='line-height:107%'>. Direction predictor</span><span lang=KO
style='line-height:107%'>와 </span><span lang=EN-US style='line-height:107%'>BTB</span><span
lang=KO style='line-height:107%'>에서 </span><span lang=EN-US style='line-height:
107%'>PC</span><span lang=KO style='line-height:107%'>값을 찾은 다음</span><span
lang=EN-US style='line-height:107%'>, BTB</span><span lang=KO style='line-height:
107%'>에</span><span lang=EN-US style='line-height:107%'> PC</span><span
lang=KO style='line-height:107%'>값을 </span><span lang=EN-US style='line-height:
107%'>index</span><span lang=KO style='line-height:107%'>로 하는 분기 목적지 주소가 존재하고 </span><span
lang=EN-US style='line-height:107%'>Direction predictor</span><span lang=KO
style='line-height:107%'>에서도 분기를 탄다고 예측을 하면 다음 </span><span lang=EN-US
style='line-height:107%'>PC</span><span lang=KO style='line-height:107%'>값으로 분기
목적지 주소를 업데이트하는 방식이다</span><span lang=EN-US style='line-height:107%'>. </span><span
lang=KO style='line-height:107%'>여기서 </span><span lang=EN-US style='line-height:
107%'>BTB</span><span lang=KO style='line-height:107%'>와 </span><span
lang=EN-US style='line-height:107%'>Direction predictor</span><span lang=KO
style='line-height:107%'>는 실제로는 </span><span lang=EN-US style='line-height:
107%'>PC</span><span lang=KO style='line-height:107%'>값 전체를 </span><span
lang=EN-US style='line-height:107%'>index</span><span lang=KO style='line-height:
107%'>로 하지 않고 </span><span lang=EN-US style='line-height:107%'>hash</span><span
lang=KO style='line-height:107%'>연산을 거쳐서 비트 수를 줄인 값을 </span><span lang=EN-US
style='line-height:107%'>index</span><span lang=KO style='line-height:107%'>로 사용한다</span><span
lang=EN-US style='line-height:107%'>. PC</span><span lang=KO style='line-height:
107%'>값 전체를 </span><span lang=EN-US style='line-height:107%'>index</span><span
lang=KO style='line-height:107%'>로 사용하게 되면 유닛의 크기가 커지고</span><span lang=EN-US
style='line-height:107%'>, </span><span lang=KO style='line-height:107%'>찾는 시간도
늘어나며 전력 소모도 커지기 때문이다</span><span lang=EN-US style='line-height:107%'>. </span><span
lang=KO style='line-height:107%'>필자가 작성한 프로그램에선 </span><span lang=EN-US
style='line-height:107%'>hash</span><span lang=KO style='line-height:107%'>연산은 구현하지
않고 </span><span lang=EN-US style='line-height:107%'>PC</span><span lang=KO
style='line-height:107%'>값 전체를 </span><span lang=EN-US style='line-height:107%'>index</span><span
lang=KO style='line-height:107%'>하는 것으로 구현하였다</span><span lang=EN-US
style='line-height:107%'>.</span></p>

<p class=MsoNormal align=center style='text-align:center'><span lang=EN-US
style='line-height:107%'><img width=295 height=221 id="그림 51"
src="README.fld/image028.jpg" alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal><span lang=KO style='line-height:107%'>위 사진은 이전 </span><span
lang=EN-US style='line-height:107%'>BTFNT</span><span lang=KO style='line-height:
107%'>의 </span><span lang=EN-US style='line-height:107%'>BTB</span><span
lang=KO style='line-height:107%'>에서 추가된 부분이다</span><span lang=EN-US
style='line-height:107%'>. Direction predictor</span><span lang=KO
style='line-height:107%'>에서 카운터의 상태에 따라 분기를 탈지 안탈지 결정하는 과정을 구현한 코드이다</span><span
lang=EN-US style='line-height:107%'>.</span></p>

<h3 style='margin-left:50.0pt;text-indent:-20.0pt'><a name="_Toc104314838"><span
lang=EN-US>Two-Level Gshare branch predictor</span></a></h3>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>One-Level branch
predictor</span><span lang=KO>에서 프로그램 전반에 걸친 분기 이력을 나타내는 </span><span
lang=EN-US>Shift </span><span lang=KO>레지스터인 </span><span lang=EN-US>Global History
Register(GHR)</span><span lang=KO>가 추가된 예측 </span><span lang=EN-US>scheme</span><span
lang=KO>이다</span><span lang=EN-US>. </span><span lang=KO>분기 명령어가 다른 분기에서 영향을 받을
수 있으므로 그 관계를 고려하여 정확도를 높이는 방식이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=281 height=156 id="그림 52" src="README.fld/image029.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><a
href="#_edn12" name="_ednref12" title=""><span class=MsoEndnoteReference><span
lang=EN-US><span class=MsoEndnoteReference><span lang=EN-US style='font-size:
10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[xii]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(Carnegie Mellon
University, 2015)</span></p>

<p class=MsoNormal><span lang=EN-US>GHR</span><span lang=KO>의 작동방식은 다음과 같다</span><span
lang=EN-US>.</span></p>

<p class=MsoListParagraph style='margin-left:28.0pt;text-indent:-18.0pt'><span
lang=EN-US style='line-height:107%'>-<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span lang=EN-US>4</span><span lang=KO>비트 </span><span
lang=EN-US>GHR </span><span lang=KO>기준 초기상태 </span><span lang=EN-US>0000 &lt;GHR
= 0000(2)&gt;</span></p>

<p class=MsoListParagraph style='margin-left:28.0pt;text-indent:-18.0pt'><span
lang=EN-US style='line-height:107%'>-<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span lang=KO style='line-height:107%'>분기 명령어 만남 </span><span
lang=EN-US style='line-height:107%'>-&gt; </span><span lang=KO
style='line-height:107%'>분기를 탔다고 가정</span><span lang=EN-US style='line-height:
107%'>, </span><span lang=KO style='line-height:107%'>마지막 비트를 </span><span
lang=EN-US style='line-height:107%'>1</span><span lang=KO style='line-height:
107%'>로 변경 </span><span lang=EN-US style='line-height:107%'>&lt;GHR = 0001(2)&gt;</span></p>

<p class=MsoListParagraph style='margin-left:28.0pt;text-indent:-18.0pt'><span
lang=EN-US style='line-height:107%'>-<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span lang=KO style='line-height:107%'>다음 분기 명령어 만남 </span><span
lang=EN-US style='line-height:107%'>-&gt; </span><span lang=KO
style='line-height:107%'>분기를 타지 않았다고 가정</span><span lang=EN-US
style='line-height:107%'>, </span><span lang=KO style='line-height:107%'>기존 값을 왼쪽으로
</span><span lang=EN-US style='line-height:107%'>1</span><span lang=KO
style='line-height:107%'>번 </span><span lang=EN-US style='line-height:107%'>shift</span><span
lang=KO style='line-height:107%'>한 뒤 마지막 비트를 </span><span lang=EN-US
style='line-height:107%'>0</span><span lang=KO style='line-height:107%'>으로 상태를 변경
</span><span lang=EN-US style='line-height:107%'>&lt;GHR = 0010(2)&gt;</span></p>

<p class=MsoListParagraph style='margin-left:28.0pt;text-indent:-18.0pt'><span
lang=EN-US style='line-height:107%'>-<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span lang=KO style='line-height:107%'>위 과정 반복</span></p>

<p class=MsoNormal><span lang=EN-US style='line-height:107%'>Gshare predictor</span><span
lang=KO style='line-height:107%'>의 </span><span lang=EN-US style='line-height:
107%'>Branch History Table(BHT=Direction predictor)</span><span lang=KO
style='line-height:107%'>의 크기는 </span><span lang=EN-US style='line-height:107%'>GHR</span><span
lang=KO style='line-height:107%'>의 비트 수에 </span><span lang=EN-US
style='line-height:107%'>2</span><span lang=KO style='line-height:107%'>를 제곱한 만큼이
된다</span><span lang=EN-US style='line-height:107%'>. </span><span lang=KO
style='line-height:107%'>필자가 작성한 프로그램에선 </span><span lang=EN-US
style='line-height:107%'>GHR</span><span lang=KO style='line-height:107%'>를 </span><span
lang=EN-US style='line-height:107%'>4</span><span lang=KO style='line-height:
107%'>비트로 설정하였으므로 </span><span lang=EN-US style='line-height:107%'>BHT</span><span
lang=KO style='line-height:107%'>의 크기 또한 </span><span lang=EN-US
style='line-height:107%'>2</span><span lang=KO style='line-height:107%'>의 </span><span
lang=EN-US style='line-height:107%'>4</span><span lang=KO style='line-height:
107%'>제곱인 </span><span lang=EN-US style='line-height:107%'>16</span><span
lang=KO style='line-height:107%'>으로 설정하였다</span><span lang=EN-US
style='line-height:107%'>. Index</span><span lang=KO style='line-height:107%'>는
</span><span lang=EN-US style='line-height:107%'>0000(2)</span><span lang=KO
style='line-height:107%'>부터 </span><span lang=EN-US style='line-height:107%'>1111(2)</span><span
lang=KO style='line-height:107%'>까지 이다</span><span lang=EN-US style='line-height:
107%'>. BHT</span><span lang=KO style='line-height:107%'>에서 카운터 값을 찾을 때는 일종의 </span><span
lang=EN-US style='line-height:107%'>hash</span><span lang=KO style='line-height:
107%'>연산을 거친다</span><span lang=EN-US style='line-height:107%'>. PC</span><span
lang=KO style='line-height:107%'>값은 최하위 </span><span lang=EN-US
style='line-height:107%'>2</span><span lang=KO style='line-height:107%'>비트가 항상 </span><span
lang=EN-US style='line-height:107%'>0</span><span lang=KO style='line-height:
107%'>이므로</span><span lang=EN-US style='line-height:107%'>, </span><span
lang=KO style='line-height:107%'>이는 </span><span lang=EN-US style='line-height:
107%'>XOR </span><span lang=KO style='line-height:107%'>연산을 할 때 의미가 없는 값이 된다</span><span
lang=EN-US style='line-height:107%'>. </span><span lang=KO style='line-height:
107%'>따라서 </span><span lang=EN-US style='line-height:107%'>PC</span><span
lang=KO style='line-height:107%'>값을 오른쪽으로 </span><span lang=EN-US
style='line-height:107%'>2</span><span lang=KO style='line-height:107%'>번 </span><span
lang=EN-US style='line-height:107%'>shift</span><span lang=KO style='line-height:
107%'>한 후 최하위 </span><span lang=EN-US style='line-height:107%'>4</span><span
lang=KO style='line-height:107%'>비트와 </span><span lang=EN-US style='line-height:
107%'>GHR</span><span lang=KO style='line-height:107%'>을 </span><span
lang=EN-US style='line-height:107%'>XOR</span><span lang=KO style='line-height:
107%'>연산</span><span lang=EN-US style='line-height:107%'>, </span><span
lang=KO style='line-height:107%'>그 값을 </span><span lang=EN-US style='line-height:
107%'>Direction predictor</span><span lang=KO style='line-height:107%'>의 </span><span
lang=EN-US style='line-height:107%'>index</span><span lang=KO style='line-height:
107%'>에서 찾는다</span><span lang=EN-US style='line-height:107%'>. </span><span
lang=KO style='line-height:107%'>아래 사진이 </span><span lang=EN-US
style='line-height:107%'>GHR</span><span lang=KO style='line-height:107%'>과 </span><span
lang=EN-US style='line-height:107%'>PC</span><span lang=KO style='line-height:
107%'>값을 합쳐서 </span><span lang=EN-US style='line-height:107%'>index</span><span
lang=KO style='line-height:107%'>로 변환하는 코드이다</span><span lang=EN-US
style='line-height:107%'>. </span></p>

<p class=MsoNormal align=center style='text-align:center'><span lang=EN-US
style='line-height:107%'><img width=352 height=69 id="그림 53"
src="README.fld/image030.jpg"></span></p>

<p class=MsoNormal><span lang=EN-US style='line-height:107%'>BTB</span><span
lang=KO style='line-height:107%'>는 이전 </span><span lang=EN-US style='line-height:
107%'>scheme</span><span lang=KO style='line-height:107%'>들과 같다</span><span
lang=EN-US style='line-height:107%'>.</span></p>

<h3 style='margin-left:50.0pt;text-indent:-20.0pt'><a name="_Toc104314839"><span
lang=EN-US>Two-Level Local branch predictor</span></a></h3>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>Gshare </span><span
lang=KO>분기 예측이 프로그램 전반에 걸친 분기 이력을 참조했다면</span><span lang=EN-US>, Local </span><span
lang=KO>분기 예측은 각 분기 명령어마다의 분기 이력을 참조하는 </span><span lang=EN-US>scheme</span><span
lang=KO>이다</span><span lang=EN-US>. Direction predictor</span><span lang=KO>의 </span><span
lang=EN-US>index</span><span lang=KO>가 </span><span lang=EN-US>PC</span><span
lang=KO>가 가리키는 명령어의 분기 이력이 된다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=317 height=185 id="그림 524" src="README.fld/image031.jpg"></span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><a
href="#_edn13" name="_ednref13" title=""><span class=MsoEndnoteReference><span
lang=EN-US><span class=MsoEndnoteReference><span lang=EN-US style='font-size:
10.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'>[xiii]</span></span></span></span></a><span
lang=EN-US style='font-size:8.0pt;line-height:107%'>(Carnegie Mellon
University, 2015)</span></p>

<p class=MsoNormal align=center style='text-align:center;text-indent:10.0pt'><span
lang=EN-US><img width=340 height=72 id="그림 523" src="README.fld/image032.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal><span lang=KO>이전 </span><span lang=EN-US>Gshare</span><span
lang=KO>에선 </span><span lang=EN-US>GHR</span><span lang=KO>과 </span><span
lang=EN-US>PC</span><span lang=KO>를 </span><span lang=EN-US>XOR</span><span
lang=KO>연산한 값이 </span><span lang=EN-US>Direction predictor</span><span lang=KO>의
</span><span lang=EN-US>index</span><span lang=KO>였지만</span><span lang=EN-US>, Local</span><span
lang=KO>에선 </span><span lang=EN-US>Local History Register(LHR)</span><span
lang=KO>가 그대로 </span><span lang=EN-US>index</span><span lang=KO>로 쓰이므로 </span><span
lang=EN-US>LHR </span><span lang=KO>배열의 분기 이력 부분을 따로 뽑아내는 과정이 필요하다</span><span
lang=EN-US>. </span><span lang=KO>위 사진이 이를 구현한 코드이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314840"><span lang=EN-US>Architectural visible state</span></a></h2>

<p class=MsoNormal align=center style='text-align:center'><span lang=EN-US><img
width=171 height=342 id="그림 15" src="README.fld/image033.jpg"
alt="텍스트이(가) 표시된 사진&#10;&#10;자동 생성된 설명"></span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>위 사진과 같이 매 </span><span
lang=EN-US>Clock Cycle</span><span lang=KO>마다 각 단계에서 처리중인 </span><span
lang=EN-US>PC, PC</span><span lang=KO>가 가리키는 명령어</span><span lang=EN-US>, Decode</span><span
lang=KO>된 명령어</span><span lang=EN-US>, </span><span lang=KO>메모리 접근</span><span
lang=EN-US>, </span><span lang=KO>레지스터 작성 등의 정보를 출력하도록 구현하였다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314841"><span lang=KO style='font-size:12.0pt;line-height:
107%'>데이터패스</span></a><span lang=EN-US style='font-size:12.0pt;line-height:
107%'>(Gshare)</span></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>지금까지 상기한 전방전달</span><span
lang=EN-US>, </span><span lang=KO>해저드 검출 유닛과 </span><span lang=EN-US>Two-Level
Gshare predictor</span><span lang=KO>를 합친 데이터패스는 다음과 같다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal align=center style='text-align:center'><span lang=EN-US><img
width=408 height=294 id="그림 54" src="README.fld/image034.jpg"></span></p>

<p class=MsoNormal><span lang=KO>빨간색으로 표시한 부분을 확대한 것은 아래와 같다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal><img width=149 height=170 src="README.fld/image035.jpg"
align=left hspace=12><img width=125 height=241 src="README.fld/image036.jpg"
align=left hspace=12><img width=219 height=251 src="README.fld/image037.jpg"
align=left hspace=12></p>

<b><span lang=EN-US style='font-size:16.0pt;line-height:107%;font-family:"맑은 고딕",sans-serif'><br
clear=all style='page-break-before:always'>
</span></b>

<p class=MsoNormal style='text-autospace:ideograph-numeric ideograph-other;
word-break:keep-all'><b><span lang=EN-US style='font-size:16.0pt;line-height:
107%'>&nbsp;</span></b></p>

<h1><a name="_Toc104314842"><b><span lang=EN-US style='font-size:16.0pt;
line-height:107%'>Conclusion</span></b></a></h1>

<p class=MsoNormal><span lang=EN-US>&nbsp;</span></p>

<h2><a name="_Toc104314843"><span lang=KO style='font-size:12.0pt;line-height:
107%'>입력 파일 별 결과 및 분석</span></a></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>먼저 분기 명령어가 존재하는 입력 파일
간 모든 분기 예측의 적중률을 비교한 그래프는 다음과 같다</span><span lang=EN-US>. </span><span lang=KO>동적
예측기는 </span><span lang=EN-US>1</span><span lang=KO>비트</span><span lang=EN-US>, 2</span><span
lang=KO>비트 </span><span lang=EN-US>Saturating, Hysteresis</span><span lang=KO>의
평균값을 표본으로 하였다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US><img width=433
height=324 id="차트 59" src="README.fld/image038.jpg"></span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>simple3 </span><span
lang=KO>파일의 경우 </span><span lang=EN-US>1</span><span lang=KO>부터 </span><span
lang=EN-US>100</span><span lang=KO>까지 더하는 </span><span lang=EN-US>for</span><span
lang=KO>문이 실행되는 코드이기 때문에 </span><span lang=EN-US>BTFNT</span><span lang=KO>의 정확도가
제일 높았다</span><span lang=EN-US>. simple4 </span><span lang=KO>파일은 </span><span
lang=EN-US>1</span><span lang=KO>부터 </span><span lang=EN-US>10</span><span
lang=KO>까지 재귀함수를 통해 더하는 코드여서 </span><span lang=EN-US>simple3</span><span
lang=KO>와는 달리 </span><span lang=EN-US>BTFNT</span><span lang=KO>의 정확도가 제일 낮은 것을
볼 수 있다</span><span lang=EN-US>. </span><span lang=KO>항상 분기를 타다가 마지막에 값을 반환할 때만 분기를
타지 않기 때문에 </span><span lang=EN-US>Always taken</span><span lang=KO>과 </span><span
lang=EN-US>One-Level</span><span lang=KO>이 제일 높은 적중률을 보였다</span><span
lang=EN-US>. fib </span><span lang=KO>파일은 분기가 상당히 복잡하고 예측 불가능하게 이루어지는 파일이다</span><span
lang=EN-US>. </span><span lang=KO>분기 명령어마다의 분기 이력을 참조하는 </span><span
lang=EN-US>Local</span><span lang=KO>이 제일 높았고</span><span lang=EN-US>, </span><span
lang=KO>다음으로 프로그램의 전체적인 분기 이력을 참조하는 </span><span lang=EN-US>Gshare</span><span
lang=KO>가 뒤를 이었다</span><span lang=EN-US>. gcd </span><span lang=KO>파일의 결과도 </span><span
lang=EN-US>fib </span><span lang=KO>만큼은 아니지만</span><span lang=EN-US>, </span><span
lang=KO>다른 파일들보다 분기의 복잡도가 높아서 정적 분기 예측보다 동적 분기 예측의 적중률이 높게 나왔다</span><span
lang=EN-US>. input4 </span><span lang=KO>파일은 총 실행된 분기 명령어의 수는 제일 많았지만</span><span
lang=EN-US>, </span><span lang=KO>거의 모든 상황에서 분기를 탔기 때문에 </span><span
lang=EN-US>Always not taken</span><span lang=KO>과 </span><span lang=EN-US>BTFNT</span><span
lang=KO>를 제외하곤 적중률이 매우 높게 나왔다</span><span lang=EN-US>. Always not-taken</span><span
lang=KO>은 </span><span lang=EN-US>0.04%</span><span lang=KO>라는 매우 낮은 수치를 보여주었다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>&nbsp;</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>다음 그래프는 위 그래프에서 동적 예측기의
예측 정확도 만을 가져온 것이다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US><img width=433
height=370 id="차트 60" src="README.fld/image039.jpg"></span></p>

<p class=MsoNormal><span lang=KO>분기를 불규칙하게 타는 </span><span lang=EN-US>fib </span><span
lang=KO>파일에서 </span><span lang=EN-US>Gshare</span><span lang=KO>의 적중률이 제일 높았다</span><span
lang=EN-US>. </span><span lang=KO>이를 통해 분기를 예측하기 복잡한 프로그램의 경우</span><span
lang=EN-US>, </span><span lang=KO>전체적인 분기 이력을 참조하여 예측하는 것이 제일 정확도가 높다는 결론을 도출했다</span><span
lang=EN-US>. </span><span lang=KO>나머지 입력 파일 들에선 </span><span lang=EN-US>input4</span><span
lang=KO>를 제외하곤 </span><span lang=EN-US>One-Level</span><span lang=KO>이 높은 적중률을 보여주었다</span><span
lang=EN-US>. </span><span lang=KO>하지만</span><span lang=EN-US>, </span><span
lang=KO>이 그래프만을 보고 </span><span lang=EN-US>One-Level</span><span lang=KO>이 평균적으로
제일 높은 적중률을 보여준다고 단언할 수는 없다</span><span lang=EN-US>. simple4 </span><span
lang=KO>파일은 실행 중 거쳐가는 분기 명령어의 수가 </span><span lang=EN-US>10</span><span
lang=KO>개에 불과하여 평균값에 포함하기엔 적절하지 않다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>다음 그래프는 분기 예측 별 적중률의
평균을 비교한 것이다</span><span lang=EN-US>. </span><span lang=KO>단</span><span
lang=EN-US>, </span><span lang=KO>위에서 언급한 것과 같이 </span><span lang=EN-US>simple4
</span><span lang=KO>파일은 실행하는 분기 명령어의 수가 </span><span lang=EN-US>10</span><span
lang=KO>개에 불과하여 예측이 틀린 횟수에 비해 적중률에 미치는 영향이 커져 제외하였다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US><img width=433
height=313 id="차트 3" src="README.fld/image040.jpg"></span></p>

<p class=MsoNormal><span lang=EN-US>2</span><span lang=KO>비트 </span><span
lang=EN-US>Hysteresis </span><span lang=KO>카운터를 적용한 </span><span lang=EN-US>Local</span><span
lang=KO>이 </span><span lang=EN-US>simple4</span><span lang=KO>를 제외한 입력 파일들을 대상으로
평균 적중률이  가장 높게 나왔다</span><span lang=EN-US>. 2</span><span lang=KO>비트 </span><span
lang=EN-US>Saturating </span><span lang=KO>카운터를 적용한 </span><span lang=EN-US>Local</span><span
lang=KO>과 </span><span lang=EN-US>2</span><span lang=KO>비트 </span><span
lang=EN-US>Hysteresis </span><span lang=KO>카운터를 적용한 </span><span lang=EN-US>Gshare</span><span
lang=KO>가 공동으로 </span><span lang=EN-US>2</span><span lang=KO>순위이고</span><span
lang=EN-US>, 2</span><span lang=KO>비트</span><span lang=EN-US> Saturating </span><span
lang=KO>카운터를 적용한 </span><span lang=EN-US>Gshare</span><span lang=KO>가 </span><span
lang=EN-US>3</span><span lang=KO>순위이다</span><span lang=EN-US>. One-Level</span><span
lang=KO>과 </span><span lang=EN-US>Gshare, Local </span><span lang=KO>각각 </span><span
lang=EN-US>3</span><span lang=KO>가지를 하나로 고려하여 분석한다면 </span><span lang=EN-US>Local
&gt; Gshare &gt; One-Level &gt; Always taken &gt; BTFNT &gt; Always not-taken </span><span
lang=KO>순으로 평균 적중률이 높게 나왔다</span><span lang=EN-US>. </span><span lang=KO>주목할 만한
점은</span><span lang=EN-US>, </span><span lang=KO>정적 분기 예측 </span><span
lang=EN-US>scheme </span><span lang=KO>중 </span><span lang=EN-US>Always-taken</span><span
lang=KO>이 꽤 높은 평균 적중률을 보였다는 것이다</span><span lang=EN-US>. Direction predictor</span><span
lang=KO>같은 별도의 분기 이력 참조 없이 </span><span lang=EN-US>BTB</span><span lang=KO>만으로도
높은 적중률을 보여준 것은 눈 여겨 볼 만 하다</span><span lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>저번 과제였던 </span><span
lang=EN-US>Single-Cycle MIPS</span><span lang=KO>에서 </span><span lang=EN-US>fib
</span><span lang=KO>파일을 실행했을 때 </span><span lang=EN-US>2679 Clock Cycle</span><span
lang=KO>이 걸렸고</span><span lang=EN-US>, </span><span lang=KO>이번 과제를 수행하며 작성한 프로그램에선
분기 예측 </span><span lang=EN-US>scheme</span><span lang=KO>에 따라 가장 높은 경우 </span><span
lang=EN-US>3125 Clock Cycle</span><span lang=KO>이</span><span lang=EN-US>, </span><span
lang=KO>가장 낮은 경우 </span><span lang=EN-US>2981 Clock Cycle</span><span lang=KO>이
걸렸다</span><span lang=EN-US>. </span><span lang=KO>비록 </span><span lang=EN-US>Single-Cycle</span><span
lang=KO>보다 사용한 </span><span lang=EN-US>Clock Cycle</span><span lang=KO>은 높아지지만</span><span
lang=EN-US>, Multi-Cycle</span><span lang=KO>이 적용된 경우 한 </span><span
lang=EN-US>Clock Cycle</span><span lang=KO>에 소모되는 시간</span><span lang=EN-US>(Clock
Cycle Time=CCT)</span><span lang=KO>이 </span><span lang=EN-US>Single-Cycle</span><span
lang=KO>보다 짧아지게 된다</span><span lang=EN-US>. </span><span lang=KO>예를 들어 </span><span
lang=EN-US>Single-Cycle</span><span lang=KO>의 </span><span lang=EN-US>CCT</span><span
lang=KO>가 </span><span lang=EN-US>lw </span><span lang=KO>명령어를 수행하는데 걸리는 시간인 </span><span
lang=EN-US>600ps</span><span lang=KO>라 한다면</span><span lang=EN-US>, Multi-Cycle</span><span
lang=KO>의 경우 </span><span lang=EN-US>CCT</span><span lang=KO>는 메모리에서 명령어를 가져오거나
데이터를 읽고 쓰는 시간인 </span><span lang=EN-US>200ps</span><span lang=KO>가 되고</span><span
lang=EN-US>, j</span><span lang=KO>와 같이 </span><span lang=EN-US>IF</span><span
lang=KO>단계에서 명령어 수행에 필요한 연산이 모두 끝나는 경우 </span><span lang=EN-US>IF</span><span
lang=KO>단계 이후로는 진행하지 않게 되어 </span><span lang=EN-US>cycle</span><span lang=KO>을 절약할
수 있다</span><span lang=EN-US>. </span><span lang=KO>여기에 파이프라인을 적용한다면</span><span
lang=EN-US>, </span><span lang=KO>모든 명령어가 </span><span lang=EN-US>5</span><span
lang=KO>단계를 거치게 되지만</span><span lang=EN-US>, </span><span lang=KO>단순 </span><span
lang=EN-US>Multi-Cycle</span><span lang=KO>의 </span><span lang=EN-US>CCT</span><span
lang=KO>를 가지고 한 </span><span lang=EN-US>cycle</span><span lang=KO>에 최대 </span><span
lang=EN-US>5</span><span lang=KO>개의 명령어를 수행 할 수 있으므로 처리율이 크게 오르게 된다</span><span
lang=EN-US>. </span><span lang=KO>위 </span><span lang=EN-US>CCT</span><span
lang=KO>를 가정하고 입력 파일 별 가장 높은 </span><span lang=EN-US>Clock Cycle</span><span
lang=KO>을 소모한 파이프라인 </span><span lang=EN-US>Single-Cycle</span><span lang=KO>의 총
</span><span lang=EN-US>CCT </span><span lang=KO>비율을 계산하면 다음 그래프와 같다</span><span
lang=EN-US>.</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US><img width=433
height=288 id="차트 8" src="README.fld/image041.jpg"></span></p>

<p class=MsoNormal><span lang=KO>위 그래프를 통해</span><span lang=EN-US> Single-Cycle</span><span
lang=KO>보다 </span><span lang=EN-US>Clock Cycle </span><span lang=KO>수가 늘어나더라도</span><span
lang=EN-US>, </span><span lang=KO>오히려 총 실행 시간은 짧아지는 파이프라인의 특성을 알아볼 수 있다</span><span
lang=EN-US>.</span></p>

<h2><a name="_Toc104314845"><span lang=KO style='font-size:12.0pt;line-height:
107%'>구현 환경</span></a></h2>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=EN-US>OS : macOS</span></p>

<p class=MsoNormal style='text-indent:10.0pt'><span lang=KO>컴파일러 </span><span
lang=EN-US>: clang 14.0.0</span></p>

</div>

</html>
