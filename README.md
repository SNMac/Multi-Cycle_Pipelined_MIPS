# Multi-Cycle_Pipelined_MIPS
Multi Cycle Pipelined MIPS processor implemented in C

파이프라이닝된 다중 사이클 MIPS 프로세서를 구현한 코드이다.
Single-Cycle_MIPS와 동일한 명령어를 지원한다.

IF/ID, ID/EX, EX/MEM, MEM/WB 총 4개의 파이프라인이 존재한다.
파이프라이닝으로 인한 해저드를 해결하기 위해 ID, EX, MEM단계에 전방전달 유닛이 추가되었고, Load-use 해저드의 해결과 ID단계에서의 정상적인 분기 연산 수행을 위해 Hazard detection unit이 추가되었다.

또한, 총 6가지의 다음 분기예측 scheme을 지원한다.

(Dynamic branch predictor)
- One-Level (1bit, 2bit-Saturating, 2bit-Hysteresis)
- Two-Level Gshare (1bit, 2bit-Saturating, 2bit-Hysteresis)
- Two-Level Local (1bit, 2bit-Saturating, 2bit-Hysteresis)

(Static branch predictor)
- Always taken
- Always not-taken
- Backward Taken, Forward Not Taken




아래 데이터패스들을 토대로 코드를 작성하였다.

<img src="https://github.com/SNMac/Multi-Cycle_Pipelined_MIPS/blob/master/Pipelined%20Datapath.jpg?raw=true">
전체 데이터패스 (Gshare)


<img src="https://github.com/SNMac/Multi-Cycle_Pipelined_MIPS/blob/master/ID%20forwarding%20unit.jpg?raw=true">
ID 전방전달 유닛


<img src="https://github.com/SNMac/Multi-Cycle_Pipelined_MIPS/blob/master/Hazard%20detection%20unit,%20MEM%20forwarding%20unit.PNG?raw=true">
해저드 검출 유닛, MEM 전방전달 유닛
