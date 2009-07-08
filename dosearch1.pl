#! /usr/bin/perl
$srcDir = shift;
$destDir = shift;
$backingDir = shift;
$childLimit = 2;

system("./treecat -r $destDir '' > /dev/null 2> tmp.holes");
@holes = `sed -n 's/missing //p' tmp.holes`;
chomp @holes;
@done = (`ls $destDir | sed -n 's/.out//p'`);
chomp @done;

foreach (@done) {
	$done{$_} = 1;
}

@seenWords = `cat allWords_s6`;
foreach (@seenWords) {
	chomp;
	$seenWord{$_} = 1;
}
#@seenWords = `sortUniq $srcDir/*out $destDir/*out`;
@seenWords = `sortUniq $destDir/*out`;
foreach (@seenWords) {
	chomp;
	($null, $count, $word) = split(/\s+/);
	next if $word =~ /\d/ || $word eq 'HOLE' || $word eq 'X';
	print $word, "\n" unless defined $seenWord{$word};
	$seenWord{$_} = 1;
}

for (;;) {
	if ($childCount >= $childLimit) {
		$donePid = wait;
		$doneHole = $pidHole{$donePid};
		print "$donePid $doneHole done\n";
		@newHoles = `perl mkHoles.pl $destDir/$doneHole.out`;
		chomp @newHoles;
		push(@holes, @newHoles);

		$numPatched = `grep -c Patched $destDir/$doneHole.err`;
		$numUnpatched = `grep -c Unpatched $destDir/$doneHole.err`;
		chomp $numPatched;
		chomp $numUnpatched;
		print "Holes: $numPatched patched, $numUnpatched unpatched\n";
		@newWords = ();
		@seenWords = `sortuniq $destDir/$doneHole.out`;
		chomp @seenWords;
		foreach (@seenWords) {
			($null, $count, $word) = split(/\s+/);
			next if $word =~ /\d/ || $word eq 'HOLE' || $word eq 'X';
			next if defined $seenWord{$word};
			$seenWord{$word} = 1;
			print "adding word $word\n";
			push(@newWords, $word);
		}
		if ($#newWords > -1) {
			open(WORDS, ">>allWords_s6");
			foreach (@newWords) {
				print WORDS "$_\n";
			}
			close WORDS;
		}
		--$childCount;
	}
	$bestHole = '1'x200;
	foreach (@holes) {
		next if defined $done{$_};
		$bestHole = $_ if (length $_) < (length $bestHole);
	}
	last if length $bestHole > 95;
	$done{$bestHole} = 1;
	++$childCount;
	$pid = fork;
	if ($pid == 0) {
		$searchWords = "-B 6";
		if ($bestHole eq '') {
			$bestHole = 'root';
			$searchWords = '';
		}
		$command = "./treecat $srcDir $bestHole | ./refine -b $bestHole -m 36 -t 6 -i 36 -s 3000000 -w allWords_s6 $searchWords -a 5.1 -M /dev/null -P twoHoleRelators > $destDir/$bestHole.out 2> $destDir/$bestHole.err";
		$first = `./treecat $srcDir $bestHole | head -1`;
		chomp $first;
		print "first = $first\n";
		if ($first eq 'HOLE') {
			$command =~ s/$srcDir/$backingDir/;
		}
		print "doing $command\n";
		system $command;
		exit;
	} else {
		$pidHole{$pid} = $bestHole;
	}
}
