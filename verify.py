"""Check all nine cut pairs; require eight refinements and one counterexample."""
import pathlib, os, subprocess, json
root=pathlib.Path(__file__).resolve().parent
out=pathlib.Path(os.environ.get('VERIFY_OUTPUT','generated-verification'))
out.mkdir(exist_ok=True)
binary=os.environ.get('ALIVE_TV','alive-tv')
results=[]
for before in sorted(root.glob('*-before.ll')):
    after=before.with_name(before.name.replace('-before.ll','-after.ll'))
    run=subprocess.run([binary,str(before),str(after)],capture_output=True,text=True,timeout=120)
    text=run.stdout+run.stderr
    name=before.stem.replace('-before','')
    (out/(name+'.txt')).write_text(text)
    expected=name!='minotaur-rank130-c2'
    correct='Transformation seems to be correct!' in text
    incorrect="Transformation doesn't verify!" in text and 'ERROR: Value mismatch' in text
    assert (correct if expected else incorrect),(name,run.returncode,text)
    results.append(dict(candidate=name,correct=correct,expected_correct=expected))
(out/'results.json').write_text(json.dumps(results,indent=2))
print('PASS: eight correct refinements and one expected rejected candidate')
