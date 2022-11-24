#!/bin/bash
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
export PATH
#dealing input
declare -i n_hosts n_players lucky_number
while getopts "m:n:l:" flag ;
do
    case $flag in
        m)
            n_hosts=$OPTARG #M
            ;;
        n)
            n_players=$OPTARG #N
            ;;
        l)
            lucky_number=$OPTARG #L
            ;;
    esac
done
#echo $M $N $L
#finish dealing input
for ((i=0;i<=n_hosts;i++));
do
mkfifo "fifo_${i}.tmp"
exec {ID}<>"fifo_${i}.tmp"
done
declare -A permu_array
declare -i games=0 #start from 1 實際個數
for ((a=1;a<=n_players;a++)); do
    for ((b=(a+1);b<=n_players;b++)); do
        for ((c=(b+1);c<=n_players;c++)); do
            for ((d=(c+1);d<=n_players;d++)); do
                for ((e=(d+1);e<=n_players;e++)); do
                    for ((f=(e+1);f<=n_players;f++)); do
                        for ((g=(f+1);g<=n_players;g++)); do
                            for ((h=(g+1);h<=n_players;h++)); do
                                permu_array[$games, 1]=$a
                                permu_array[$games, 2]=$b
                                permu_array[$games, 3]=$c
                                permu_array[$games, 4]=$d
                                permu_array[$games, 5]=$e
                                permu_array[$games, 6]=$f
                                permu_array[$games, 7]=$g
                                permu_array[$games, 8]=$h
                                #echo ${permu_array[$games,1]} ${permu_array[$games,2]} ${permu_array[$games,3]} ${permu_array[$games,4]} ${permu_array[$games,5]} ${permu_array[$games,6]} ${permu_array[$games,7]} ${permu_array[$games,8]}
                                ((games=games+1))
                            done
                        done
                    done
                done
            done
        done
    done
done

#for ((i=0;i<games;i++)) do
#    echo $((permu_array[$i,1])) $((permu_array[$i,2])) $((permu_array[$i,3])) $((permu_array[$i,4])) $((permu_array[$i,5])) $((permu_array[$i,6])) $((permu_array[$i,7])) $((permu_array[$i,8]))
#done
#exit 0

#if m=10 n=8
declare -i goout=0 comein=0
declare -A players_points #注意index
#init
for ((a=1;a<=n_players;a++)); do
    players_points[$a]=0
done
#one by one send message
#之後認真檢查這裡有沒有問題 read too many times
for ((a=1;a<=$n_hosts;a++)); do #發工作
    printf -v giveoutname 'fifo_%d.tmp' "$a"
    #echo "$((permu_array[$goout,1])) $((permu_array[$goout,2])) $((permu_array[$goout,3])) $((permu_array[$goout,4])) $((permu_array[$goout,5])) $((permu_array[$goout,6])) $((permu_array[$goout,7])) $((permu_array[$goout,8]))"
    echo "$((permu_array[$goout, 1])) $((permu_array[$goout, 2])) $((permu_array[$goout, 3])) $((permu_array[$goout, 4])) $((permu_array[$goout, 5])) $((permu_array[$goout, 6])) $((permu_array[$goout, 7])) $((permu_array[$goout, 8]))">$giveoutname & 
    ./host -m $a -d 0 -l $lucky_number &
    ((goout=goout+1))
    if [ $goout -eq $games ]; then
    break
    fi
done
pathin="fifo_0.tmp"
# bash chiffon.sh -m 4 -n 9 -l 730 出大事
while [ $comein -lt $games ];
do
#echo "eses"
#declare -i comeinhost
read -r comeinhost #知道是哪個host
#echo $comeinhost

#declare -i ID score
for ((a=1;a<9;a++)); do #讀player行+分數
    read -r line
    #echo $a $line
    IFS=" " read id score <<< ${line}
    #echo $id $score
    players_points[$id]=$((players_points[$id]+score))
done
printf -v giveoutname  'fifo_%d.tmp' "$comeinhost"
if [ $goout -eq $games ]; then
    echo "-1 -1 -1 -1 -1 -1 -1 -1">$giveoutname & #只需要回收東西 發送-1
else
    #echo "$((permu_array[$goout,1])) $((permu_array[$goout,2])) $((permu_array[$goout,3])) $((permu_array[$goout,4])) $((permu_array[$goout,5])) $((permu_array[$goout,6])) $((permu_array[$goout,7])) $((permu_array[$goout,8]))"
    echo "$((permu_array[$goout, 1])) $((permu_array[$goout, 2])) $((permu_array[$goout, 3])) $((permu_array[$goout, 4])) $((permu_array[$goout, 5])) $((permu_array[$goout, 6])) $((permu_array[$goout, 7])) $((permu_array[$goout, 8]))">$giveoutname & 
    ((goout=goout+1)) #回收並發送
fi
((comein=comein+1))
done < ${pathin}

#test point!!!
#for ((i=1;i<=n_players;i++)); do
#    echo $((players_points[$i]))
#done
#exit 0
#test point!!!

#print finalranking
declare -A finalarray finalprint
for ((i=1;i<=n_players;i++));
do
((finalarray[$i, 1]=players_points[$i]))
((finalarray[$i, 0]=$i))
((finalprint[$i, 0]=$i))
done

#doing bubble
declare -i tmpone tmptwo
for ((i=1;i<=n_players-1;i++)); do
    for ((j=$i+1;j<=n_players;j++)); do
        if [ $((finalarray[$i, 1])) -le $((finalarray[$j, 1])) ]; then
            ((tmpone=finalarray[$i, 0]))
            ((finalarray[$i, 0]=finalarray[$j, 0]))
            ((finalarray[$j, 0]=tmpone))
            ((tmptwo=finalarray[$i, 1]))
            ((finalarray[$i, 1]=finalarray[$j, 1]))
            ((finalarray[$j, 1]=tmptwo))
        fi
    done
done
#doing bubble


declare -i ranking=1 adding=1 last
for ((i=1;i<=n_players;i++)); do
    declare -i var=$((finalarray[$i, 0])) onlyone=1
    #echo $i $onlyone
    if [ $i -eq $onlyone ]; then
        ((finalprint[$var, 0]=finalarray[${i}, 0]))
        ((finalprint[$var, 1]=ranking))
    else
        if [ $last -ne $((finalarray[$i, 1])) ]; then
            ((ranking=adding))
        fi
        ((finalprint[$var, 0]=finalarray[${i}, 0]))
        ((finalprint[$var, 1]=ranking))
    fi
    #echo $((finalprint[$var,0])) $((finalprint[$var,1]))
    ((last=finalarray[$i, 1]))
    ((adding=adding+1))
done
#print finalranking
#finalprint
for ((i=1;i<=n_players;i++)); do
    echo $((finalprint[$i, 0])) $((finalprint[$i, 1]))
done
#finalprint
#rm fifo
for ((i=0;i<=n_hosts;i++));
do
rm "fifo_${i}.tmp"
done
#rm fifo
#wait
#for ((i=0;i<n_hosts;i++));
#do
wait
#done
#wait
exit 0