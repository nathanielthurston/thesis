while (<>) {
	chomp;
	if (/(\w+)\(([01]+)\) = /) {
#		print "$_ BW\n";
		$word = $1;
		$box = $2;
		$matrixIndex = 0;
		$idMatrix = 'Y';
		if ($box ne $lastBox) {
			print "\n" unless $lastBox eq '';
			print "$box ";
			$lastBox = $box;
		}
	}
	if (/err=.* size=.* abs=\[ *(.+), *(.+)\]/) {
		$min = $1; $max = $2;
		if (($matrixIndex == 0 || $matrixIndex == 3) && ($min > 1 || $max < 1)
		 || ($matrixIndex == 1 || $matrixIndex == 2) && ($min > 0)) {
			if ($matrixIndex == 2 && $idMatrix eq 'Y') {
				$idMatrix = '*';
			} else {
				$idMatrix = 'N';
			}
		}
#		print "$_ A($1,$2) $idMatrix\n";
		++$matrixIndex;
		if ($matrixIndex == 4) {
			print $idMatrix;
		}
	}
}

print "\n";
