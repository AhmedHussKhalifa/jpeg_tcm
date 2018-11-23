
i=1
for d in */ ; do
    echo "$d"
    mv $d $i
    let i=i+1
done