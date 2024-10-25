import os


def create_job_file(job_name, test_file, i):
	program_command = f"./prova2 {test_file}\n"
	with open(f"{job_name}.sh", "w") as f:
		f.write(f"#!/bin/bash\n")
		f.write(f"#SBATCH --job-name=prova_{i}\n")
		f.write(f"#SBATCH --partition=arrow\n")
		f.write(f"#SBATCH --ntasks=1\n")
		f.write(f"#SBATCH --mem=14GB\n")
		f.write(f"# warm up processors\n")
		f.write(f"sudo cpupower frequency-set -g performance\n")
		f.write(f"sleep 0.1\n")
		f.write(f"stress-ng -c 4 --cpu-ops=100\n")
		f.write(f"# set limits\n")
		f.write(f"ulimit -v 16777216\n")
		f.write(f"######################\n")
		f.write(program_command)
		f.write(f"######################\n")
		f.write(f"# back to power saving mode\n")
		f.write(f"sudo cpupower frequency-set -g powersave\n")


def main():
	test_file = ["30n20b8.mps.gz", "50v-10.mps.gz", "academictimetablesmall.mps.gz", "air05.mps.gz",
	 "app1-1.mps.gz", "app1-2.mps.gz", "assign1-5-8.mps.gz", "atlanta-ip.mps.gz", "b1c1s1.mps.gz", "bab2.mps.gz",
	  "bab6.mps.gz", "beasleyC3.mps.gz", "binkar10_1.mps.gz", "blp-ar98.mps.gz", "blp-ic98.mps.gz", "bnatt400.mps.gz",
	  "bnatt500.mps.gz", "bppc4-08.mps.gz", "brazil3.mps.gz", "buildingenergy.mps.gz", "cbs-cta.mps.gz", "chromaticindex512-7.mps.gz",
	  "chromaticindex1024-7.mps.gz", "cmflsp50-24-8-8.mps.gz", "CMS750.mps.gz", "co-100.mps.gz", "cod105.mps.gz", "comp07-2idx.mps.gz", "comp21-2idx.mps.gz",
	  "cost266-UUE.mps.gz", "cryptanalysiskb128n5obj14.mps.gz", "cryptanalysiskb128n5obj16.mps.gz", "csched007.mps.gz", "csched008.mps.gz",
	  "cvs16r128-89.mps.gz", "dano3_3.mps.gz", "dano3_5.mps.gz", "decomp2.mps.gz", "drayage-25-23.mps.gz", "drayage-100-23.mps.gz", "dws008-01.mps.gz", 
	  "eil33-2.mps.gz", "eilA101-2.mps.gz", "enlight_hard.mps.gz", "ex9.mps.gz", "ex10.mps.gz", "exp-1-500-5-5.mps.gz", "fast0507.mps.gz", "fastxgemm-n2r6s0t2.mps.gz",
	  "fhnw-binpack4-4.mps.gz", "fhnw-binpack4-48.mps.gz", "fiball.mps.gz", "gen-ip002.mps.gz", "gen-ip054.mps.gz", "germanrr.mps.gz", "gfd-schedulen180f7d50m30k18.mps.gz", "glass4.mps.gz",
	  "glass-sc.mps.gz", "gmu-35-40.mps.gz", "gmu-35-50.mps.gz", "graph20-20-1rand.mps.gz", "graphdraw-domain.mps.gz", "h80x6320d.mps.gz", "highschool1-aigio.mps.gz", "hypothyroid-k1.mps.gz",
	  "ic97_potential.mps.gz", "icir97_tension.mps.gz", "irish-electricity.mps.gz", "irp.mps.gz", "instanbul-no-cutoff.mps.gz", "k1mushroom.mps.gz", "lectsched-5-obj.mps.gz", "leo1.mps.gz", "leo2.mps.gz",
	  "lotsize.mps.gz", "mad.mps.gz", "map10.mps.gz", "map16715-04.mps.gz", "markshare2.mps.gz", "markshare_4_0.mps.gz", "mas74.mps.gz", "mas76.mps.gz", "mc11.mps.gz", "mcsched.mps.gz", "mik-250-20-75-4.mps.gz",
	  "milo-v12-6-r2-40-1.mps.gz", "momentum1.mps.gz", "mushroom-best.mps.gz", "mzzv11.mps.gz", "mzzv42z.mps.gz", "n2seq36q.mps.gz", "n3div36.mps.gz", "n5-3.mps.gz", "neos5.mps.gz", "neos8.mps.gz", "neos17.mps.gz",
	  "neos859080.mps.gz", "neos-631710.mps.gz", "neos-662469.mps.gz", "neos-787933.mps.gz", "neos-827175.mps.gz", "neos-848589.mps.gz", "neos-860300.mps.gz", "neos-873061.mps.gz", "neos-911970.mps.gz", "neos-933966.mps.gz",
	  "neos-950242.mps.gz", "neos-957323.mps.gz", "neos-960392.mps.gz", "neos-1122047.mps.gz", "neos-1171448.mps.gz", "neos-1171737.mps.gz", "neos-1354092.mps.gz", "neos-1445765.mps.gz", "neos-1456979.mps.gz", "neos-1582420.mps.gz",
	  "neos-2075418-temuka.mps.gz", "neos-2657525-crna.mps.gz", "neos-2746589-doon.mps.gz", "neos-2978193-inde.mps.gz", "neos-2987310-joes.mps.gz", "neos-3004026-krka.mps.gz", "neos-3024952-loue.mps.gz", "neos-3046615-murg.mps.gz",
	  "neos-3083819-nubu.mps.gz", "neos-3216931-puriri.mps.gz", "neos-3381206-awhea.mps.gz", "neos-3402294-bobin.mps.gz", "neos-3402454-bohle.mps.gz", "neos-3555904-turama.mps.gz",
	  "neos-3627168-kasai.mps.gz", "neos-3656078-kumeu.mps.gz", "neos-3754480-nidda.mps.gz", "neos-3988577-wolgan.mps.gz", "neos-4300652-rahue.mps.gz", "neos-4338804-snowy.mps.gz", "neos-4387871-tavua.mps.gz", "neos-4413714-turia.mps.gz", "neos-4532248-waihi.mps.gz",
	  "neos-4647030-tutaki.mps.gz", "neos-4722843-widden.mps.gz", "neos-4738912-atrato.mps.gz", "neos-4763324-toguru.mps.gz", "neos-4954672-berkel.mps.gz", "neos-5049753-cuanza.mps.gz", "neos-5052403-cygnet.mps.gz",
	  "neos-5093327-huahum.mps.gz", "neos-5104907-jarama.mps.gz", "neos-5107597-kakapo.mps.gz", "neos-5114902-kasavu.mps.gz", "neos-5188808-nattai.mps.gz", "neos-5195221-niemur.mps.gz", "net12.mps.gz", "netdiversion.mps.gz", "nexp-150-20-8-5.mps.gz", "ns1116954.mps.gz", 
	  "ns1208400.mps.gz", "ns1644855.mps.gz", "ns1760995.mps.gz", "ns1830653.mps.gz", "ns1952667.mps.gz", "nu25-pr12.mps.gz", "nursesched-medium-hint03.mps.gz", "nursesched-sprint02.mps.gz", "nw04.mps.gz", "opm2-z10-s4.mps.gz", "p200x1188c.mps.gz", "peg-solitaire-a3.mps.gz",
	  "pg.mps.gz", "pg5_34.mps.gz", "physiciansched3-3.mps.gz", "physiciansched6-2.mps.gz", "piperout-08.mps.gz", "piperout-27.mps.gz", "pk1.mps.gz", "proteindesign121hz512p9.mps.gz", "proteindesign122trx11p8.mps.gz", "qap10.mps.gz", "radiationm18-12-05.mps.gz", 
	  "radiationm40-10-02.mps.gz", "rail01.mps.gz", "rail02.mps.gz", "rail507.mps.gz", "ran14x18-disj-8.mps.gz", "rd-rplusc-21.mps.gz", "reblock115.mps.gz", "rmatr100-p10.mps.gz", "rmatr200-p5.mps.gz", "rocl-4-11.mps.gz", "rocll-5-11.mps.gz", "rococoB10-011000.mps.gz", "rococoC10-001000.mps.gz",
	   "roi2alpha3n4.mps.gz", "roi5alpha10n8.mps.gz", "roll3000.mps.gz", "s100.mps.gz", "s250r10.mps.gz", "satellites2-40.mps.gz", "satellites2-60-fs.mps.gz", "savsched1.mps.gz", "sct2.mps.gz", "seymour.mps.gz", "seymour1.mps.gz", "sing44.mps.gz", "sing326.mps.gz", "snp-02-004-104.mps.gz","sorrell3.mps.gz", 
	   "sp97ar.mps.gz", "sp98ar.mps.gz", "sp150x300d.mps.gz", "splice1k1.mps.gz", "square41.mps.gz", "square47.mps.gz", "supportcase6.mps.gz", "supportcase7.mps.gz", "supportcase10.mps.gz", "supportcase12.mps.gz", "supportcase18.mps.gz", 
	   "supportcase19.mps.gz", "supportcase22.mps.gz", "supportcase26.mps.gz", "supportcase33.mps.gz", "supportcase40.mps.gz", "supportcase42.mps.gz", "swath1.mps.gz", "swath3.mps.gz", "tbfp-network.mps.gz", "thor50dday.mps.gz", "timtab1.mps.gz", "tr12-30.mps.gz", "traininstance2.mps.gz", "traininstance6.mps.gz",
	   "trento1.mps.gz", "triptim1.mps.gz", "uccase9.mps.gz", "uccase12.mps.gz", "uct-subprob.mps.gz", "unitcal_7.mps.gz", "var-smallemery-m6j6.mps.gz", "wachplan.mps.gz"  ]

	for i, test_file in enumerate(test_file, start = 1):
		job_name = f"job_{i}"
		create_job_file(job_name, test_file, i)


	print("File creati con successo\n")



if __name__ == "__main__":
	main()