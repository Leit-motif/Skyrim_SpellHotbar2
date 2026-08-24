#include "combo_probe.h"
#include <string>
#include <Windows.h>
#include "../logger/logger.h"

namespace SpellHotbar::casts::ComboProbe {

	namespace {
		constexpr const char* kNextAttack{ "MCO_nextattack" };
		constexpr const char* kNextPowerAttack{ "MCO_nextpowerattack" };

		const char* probe_ini()
		{
			return "Data\\SKSE\\Plugins\\SpellHotbar2_ComboProbe.ini";
		}

		void append_int(std::string& out, int value)
		{
			out += std::to_string(value);
		}

		// A graph that refuses the read is reported as `?`, never dropped: a graph that does not
		// carry the variable is itself the answer this probe exists to find.
		void append_graph_values(std::string& out, RE::BShkbAnimationGraph* graph)
		{
			int next = 0;
			int power = 0;
			const bool ok_next = graph->GetGraphVariableInt(kNextAttack, next);
			const bool ok_power = graph->GetGraphVariableInt(kNextPowerAttack, power);
			out += " n=";
			if (ok_next) {
				append_int(out, next);
			} else {
				out += '?';
			}
			out += " p=";
			if (ok_power) {
				append_int(out, power);
			} else {
				out += '?';
			}
		}

		// The holder-level read the driver's own write_mco uses, for comparison against the
		// per-graph values on the same line.
		void append_holder_values(std::string& out, RE::Actor* actor)
		{
			std::int32_t next = 0;
			std::int32_t power = 0;
			const bool ok_next = actor->GetGraphVariableInt(kNextAttack, next);
			const bool ok_power = actor->GetGraphVariableInt(kNextPowerAttack, power);
			out += " holder n=";
			if (ok_next) {
				append_int(out, next);
			} else {
				out += '?';
			}
			out += " p=";
			if (ok_power) {
				append_int(out, power);
			} else {
				out += '?';
			}
		}

		// Runs on the animation and input threads, so it takes no lock of its own and does no
		// work beyond the reads and one string.
		std::string describe_graphs(RE::Actor* actor, bool write_first, int nextAttack,
			int nextPowerAttack)
		{
			std::string out;
			out.reserve(256);
			if (!actor) {
				return "<no actor>";
			}
			RE::BSTSmartPointer<RE::BSAnimationGraphManager> manager;
			if (!actor->GetAnimationGraphManager(manager) || !manager) {
				return "<no graph manager>";
			}
			const std::uint32_t active = manager->GetRuntimeData().activeGraph;
			const std::uint32_t count = static_cast<std::uint32_t>(manager->graphs.size());
			out += "graphs=";
			append_int(out, static_cast<int>(count));
			for (std::uint32_t i = 0; i < count; ++i) {
				auto* graph = manager->graphs[i].get();
				out += " [";
				append_int(out, static_cast<int>(i));
				if (i == active) {
					out += '*';
				}
				out += ' ';
				if (!graph) {
					out += "<null>]";
					continue;
				}
				out += graph->projectName.c_str() ? graph->projectName.c_str() : "<unnamed>";
				if (write_first) {
					const bool wrote_next = graph->SetGraphVariableInt(kNextAttack, nextAttack);
					const bool wrote_power =
						graph->SetGraphVariableInt(kNextPowerAttack, nextPowerAttack);
					out += wrote_next ? " w=1" : " w=0";
					out += wrote_power ? "/1" : "/0";
				}
				append_graph_values(out, graph);
				out += ']';
			}
			append_holder_values(out, actor);
			return out;
		}
	}

	int mode()
	{
		// Same flush-then-read as load_charge_curve: the profile cache would otherwise hand back
		// the value from the first read for the whole session.
		WritePrivateProfileStringA(nullptr, nullptr, nullptr, nullptr);
		return static_cast<int>(GetPrivateProfileIntA("Probe", "iMode", 0, probe_ini()));
	}

	void probe_read_graphs(RE::Actor* actor, std::string_view where)
	{
		logger::info("SH2 probe: {} | {}", where, describe_graphs(actor, false, 0, 0));
	}

	void probe_write_graphs(RE::Actor* actor, int nextAttack, int nextPowerAttack,
		std::string_view where)
	{
		logger::info("SH2 probe: {} next={} power={} | {}", where, nextAttack, nextPowerAttack,
			describe_graphs(actor, true, nextAttack, nextPowerAttack));
	}
}
