"""Recalculate original paired statistics and corpus counts using Python stdlib."""
import csv,pathlib,statistics,random,json,collections,sys
root=pathlib.Path(__file__).resolve().parent
def read(p):
    with p.open(newline='') as f:return list(csv.DictReader(f))
data=[r for n in [2,3,4] for r in read(root/f'results-run{n}.csv')]
assert len(data)==504
corpus=read(root/'corpus.csv');groups={r['ir_identity'] for r in corpus}
assert len(corpus)==99 and len(groups)==87
seen=set();cuts=0
for r in corpus:
    if r['recorded_status']=='COMPLETE' and r['ir_identity'] not in seen:
        seen.add(r['ir_identity']);cuts+=int(r['recorded_cuts'])
assert cuts==1667
output=[]
for candidate in sorted({r['candidate'] for r in data}):
    rs=[r for r in data if r['candidate']==candidate];assert len(rs)==63
    b=[int(r['before_ns'])/int(r['iterations']) for r in rs]
    a=[int(r['after_ns'])/int(r['iterations']) for r in rs]
    reductions=[100*(1-int(r['after_ns'])/int(r['before_ns'])) for r in rs]
    rng=random.Random(20261015)
    boot=sorted(statistics.median(rng.choices(reductions,k=63)) for _ in range(10000))
    output.append(dict(candidate=candidate,samples=63,before_ns=statistics.median(b),after_ns=statistics.median(a),median_paired_reduction_pct=statistics.median(reductions),ci95_low=boot[249],ci95_high=boot[9749],wins=sum(x<y for x,y in zip(a,b))))
assert sum(r['wins'] for r in output)==501
print(json.dumps(dict(positions=99,unique_functions=87,repeats=12,cuts=cuts,paired_wins=501,paired_total=504,metrics=output),indent=2))
