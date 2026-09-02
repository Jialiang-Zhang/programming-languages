# 来源、核验与局限

## 来源层级

### A. 官方/准官方：考试时间、形式与趋势经验

1. CCF CSP 官方通知列表：  
   https://www.cspro.org/cms/show.action?channelid=0000000103&code=publish_8ac21fad9d27f22a019f64d8f05e0113
2. CCF 认证计划查询：  
   https://csp.ccf.org.cn/csp/show.action?_ack=1&code=exam_plan_seach
3. CCF《CSP题型分析与考场应对策略》（2026-03-13）：  
   https://www.ccf.org.cn/Focus/2026-03-13/861608.shtml
4. CCF《CSP考场高效得分策略分享》（2026-03-05）：  
   https://www.ccf.org.cn/ccsp/Bulletin/2026-03-05/861525.shtml
5. CCF 历次真题精讲/高分经验栏目：用于核验近年题型趋势和部分分策略。

### B. 题名与公开实现交叉核验

1. qzylalala/CSP：  
   https://github.com/qzylalala/CSP
2. van-Bo/CSP-problem-solutions：  
   https://github.com/van-Bo/CSP-problem-solutions
3. rucdatascience/CCF-CSP-solutions：  
   https://github.com/rucdatascience/CCF-CSP-solutions
4. 曙梦 OJ 的 CSP 往届复刻题单：  
   https://oj.shumeng.tech/

### C. 历史算法标签补充

1. 阿里云开发者社区《CCF-CSP 历年考点全归纳》：  
   https://developer.aliyun.com/article/1003316
2. 公开题解、参赛复盘和代码仓库：只用于交叉核验代表性做法，不把任何单篇社区题解视作“官方唯一正解”。

## 核验规则

- 题名至少用两个公开列表或“列表 + 代码目录”交叉核验；近年题名优先使用当前仍更新的题单。
- 算法标签优先记录多个题解一致的主方法；存在明显分歧时只写宽算法族。
- 一道题有多种可通过算法时，记录的是适合复习索引的代表性组合，而不是排他性答案。
- 第4、5题常有按子任务设计的多级方案；因此数据中的“做题方法”刻意保留“先朴素、再优化”的入口。
- 只整理题名和原创归纳，不收录完整题面。

## 已知局限

1. CCF 并未为每道题发布统一的“算法标签”，标签必然带有人工归类成分。
2. 极少数高难题的完整公开题解较少，已标记为低置信，不应据此断言唯一满分算法。
3. 第42次实际考试日期为 2026-05-31，社区资料中同时出现 `202605` 与 `202606` 编号；JSONL 同时保留主编号和别名。
4. 预测模型只学习“题位—算法族”的历史分布，没有使用未公开信息，也不能预测具体题面。
