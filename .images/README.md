# Modern C++
[Smart Pointers Video](https://www.youtube.com/watch?v=UOB7-B2MfwA&list=PLlrATfBNZ98dudnM48yfGUldqGD0S4FFb&index=45)

<!-- <img width="1244" height="309" alt="image" src="https://github.com/user-attachments/assets/c77a0927-ed04-419a-ba20-ee5c6d003038" /> -->
Unique pointer is destroyed after its scope ends. You can't assign another object to an existing unique pointer

<img width="1258" height="500" alt="image" src="https://github.com/user-attachments/assets/1eb9eb20-232b-4af0-875d-efa213d76473" />

Shared pointer is destroyed after its reference count becomes 0

<img width="1136" height="480" alt="image" src="https://github.com/user-attachments/assets/5ac684a8-43f8-4ab7-8e00-c330b8f84d9c" />

Weak pointers

<img width="1333" height="582" alt="image" src="https://github.com/user-attachments/assets/a5d4cf46-0484-448b-ae70-a4a2cfecab2e" />

# cmake
![sasas](cmake.png)

# regex
![sasas](regex_func.png)

# lexer 
![sasas](token.png)
![sasas](regex_cheat_sheet.png)
![sasas](lexical_analysis.png)
![sasas](lex_regex_nfa_dfa_table.png)

# parser
<img width="1026" height="394" alt="image" src="https://github.com/user-attachments/assets/dd21dce3-c87d-4801-9661-1bb91a41f15d" />
<img width="1050" height="533" alt="image" src="https://github.com/user-attachments/assets/1bfd0d08-3550-4d60-9b67-753b652baaee" />
<img width="1703" height="637" alt="image" src="https://github.com/user-attachments/assets/2b75fc1d-2bac-483c-a8b0-6e52526d122f" />

<img width="1658" height="803" alt="image" src="https://github.com/user-attachments/assets/71eac9fb-9923-43e4-9d62-a312c08adc90" />
<img width="1031" height="474" alt="image" src="https://github.com/user-attachments/assets/f7c88d59-50bb-47d1-bc10-ee927f896744" />
<img width="1028" height="465" alt="image" src="https://github.com/user-attachments/assets/d82092be-0474-4a0a-b495-545aba26dbd8" />
<img width="1020" height="437" alt="image" src="https://github.com/user-attachments/assets/a9f5062a-a4a6-4c2e-8227-7427a210b22e" />
<img width="1098" height="584" alt="image" src="https://github.com/user-attachments/assets/5ee40848-f73a-4b71-b1d0-75b0d16832eb" />

<img width="1023" height="439" alt="image" src="https://github.com/user-attachments/assets/6f19e334-f328-4870-bebf-be50ff88f25b" />

What we want is for the `else`s to associate with the closest matching `if`


<img width="955" height="544" alt="image" src="https://github.com/user-attachments/assets/649d7b1b-0df5-4d52-9579-35cd374a46fa" />

<img width="955" height="578" alt="image" src="https://github.com/user-attachments/assets/cabbfc92-5f7f-4150-bbfc-bfcf28869041" />

<img width="1026" height="501" alt="image" src="https://github.com/user-attachments/assets/9bb0ea25-bd77-451a-b79f-17510abd441c" />

<img width="1034" height="537" alt="image" src="https://github.com/user-attachments/assets/488151cb-639b-4c62-a70a-e74c9c00cb75" />

## Abstract Syntax Tree
<img width="979" height="495" alt="image" src="https://github.com/user-attachments/assets/fa7a9732-abb1-424e-a02e-91c65bb751ac" />
<img width="1123" height="612" alt="image" src="https://github.com/user-attachments/assets/bcac961f-a51e-41bb-8ee3-4bf0e63df23e" />
<img width="760" height="497" alt="image" src="https://github.com/user-attachments/assets/23c6f3e7-a4f8-4b09-a91b-16fc21bc0927" />

## Recursive Descent Parsing (top-down parsing algorithm)
<img width="1054" height="477" alt="image" src="https://github.com/user-attachments/assets/3526762e-5113-41c2-b2c7-462a5946254d" />

<img width="1018" height="504" alt="image" src="https://github.com/user-attachments/assets/0b7a7c18-738b-438a-b695-05514b1bb922" />
<img width="1004" height="494" alt="image" src="https://github.com/user-attachments/assets/c54112d4-13f6-4193-abdf-03d295fca1bd" />

---
Match!

<img width="891" height="484" alt="image" src="https://github.com/user-attachments/assets/3d15a47a-ff89-40e8-a5f5-d488a59f918f" />

...

<img width="877" height="506" alt="image" src="https://github.com/user-attachments/assets/7936ac18-a1f9-4d13-a1e9-d6ef98b739fc" />

<img width="1053" height="564" alt="image" src="https://github.com/user-attachments/assets/65417d98-1aa6-4b13-b26a-cfffbe6580cd" />

<img width="1133" height="570" alt="image" src="https://github.com/user-attachments/assets/442c7d4c-f5ae-4804-8112-af57d31389f8" />

<img width="1040" height="565" alt="image" src="https://github.com/user-attachments/assets/3e185e47-6042-427a-be34-6dbb4533b1dd" />

<img width="1057" height="584" alt="image" src="https://github.com/user-attachments/assets/760deb0c-2113-460d-bcb3-d240f3a22fda" />

<img width="1725" height="910" alt="image" src="https://github.com/user-attachments/assets/b1f0717a-c9ff-4521-99de-06c4d17bc6d8" />

---

Left-recursive grammar does it from right to left, but Recursive Descent Parsing works from left to right, so Left-recursive grammar doesn't work for Recursive Descent Parsing\
<img width="1103" height="502" alt="image" src="https://github.com/user-attachments/assets/a3309735-488a-49ea-9f9e-604a7c8bf979" />

<img width="859" height="498" alt="image" src="https://github.com/user-attachments/assets/ba7c2dbf-41be-46f9-a1a8-64607f7d4cd3" />

<img width="813" height="302" alt="image" src="https://github.com/user-attachments/assets/8a90d5c6-5d3a-45a2-8c13-843e27265006" />

---
## LL LR Parsing

<img width="1088" height="554" alt="image" src="https://github.com/user-attachments/assets/d85ed233-ded9-42b5-8bad-73bead5a5646" />

<img width="913" height="204" alt="image" src="https://github.com/user-attachments/assets/65c4138f-d362-4ee9-88d8-df59ec45595a" />

### Predictive Parsing

> The general idea is we factor out the common prefix, one production deals with the prefix, and then we introduce a new terminal for the suffixes and just have multiple productions, one for each possible suffix. Delays decision about which production we're using, so instead of having to decide immediately which production to use 

<img width="565" height="489" alt="image" src="https://github.com/user-attachments/assets/dddd71b8-4ab3-49d9-8eae-7a89dc2fdc95" />

<img width="1060" height="601" alt="image" src="https://github.com/user-attachments/assets/361a2ea0-a339-45eb-b3da-cb3b85bd8275" />

<img width="1061" height="549" alt="image" src="https://github.com/user-attachments/assets/a17c1aab-4701-4ccd-a99a-ac152782c4b0" />

<img width="1020" height="585" alt="image" src="https://github.com/user-attachments/assets/3fadfeac-fc72-494a-9073-fec8885defaa" />

<img width="927" height="463" alt="image" src="https://github.com/user-attachments/assets/e0fb269d-fae6-4949-ad5b-6517e0466273" />

<img width="1095" height="596" alt="image" src="https://github.com/user-attachments/assets/98ee7549-7ba9-4eb4-9879-dc052d710719" />

---

<img width="1911" height="933" alt="image" src="https://github.com/user-attachments/assets/6a8ec21c-bc36-4b51-9213-3928c1241868" />

#### First Sets

<img width="1656" height="1080" alt="image" src="https://github.com/user-attachments/assets/68c2bd5a-9853-4b8a-9615-63de08c7f0ef" />


<img width="1523" height="727" alt="image" src="https://github.com/user-attachments/assets/4fe107bc-dc2e-42aa-843a-279fb45eba99" />

But since `T` doesn't have a terminal, we can say `First(E) = First(T)` instead of `First(T)` being a subset of `First(E)` 

<img width="1523" height="727" alt="image" src="https://github.com/user-attachments/assets/e303cae0-eff5-4347-8d7a-4d0fe0028eca" />

#### Follow Sets

<img width="1265" height="642" alt="image" src="https://github.com/user-attachments/assets/744abc1c-3b54-403d-806b-af96b0fc7594" />

<img width="1505" height="894" alt="image" src="https://github.com/user-attachments/assets/a26bee3b-de42-4936-b827-e66a116d23ed" />

We don't include `eps` in follow sets, but consider for **computing** follow sets

<img width="1836" height="908" alt="image" src="https://github.com/user-attachments/assets/77ee3d42-7cd2-4508-aa52-e7be07a8c27c" />

<img width="1836" height="908" alt="image" src="https://github.com/user-attachments/assets/674a81a6-4a51-4434-8c3b-47f2766cfe8b" />

<img width="1769" height="676" alt="image" src="https://github.com/user-attachments/assets/270631ec-573c-4d9a-82f1-530a19a2e117" />

<img width="1769" height="676" alt="image" src="https://github.com/user-attachments/assets/e08865f9-27d4-47c6-a524-df2f171afc99" />

<img width="1769" height="676" alt="image" src="https://github.com/user-attachments/assets/7136e36d-2339-4e3e-8e3b-f2dfc11df9a0" />
